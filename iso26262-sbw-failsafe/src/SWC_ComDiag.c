
#include "Rte_SWC_ComDiag.h"

/* Alive Counter 연속 오류 허용 횟수 */
#define SEQUENCE_ERROR_LIMIT     (2U)

/* FAIL-SAFE 이후 연속 증가 확인 횟수 */
#define RECOVERY_VALID_LIMIT     (3U)

/*
 * Alive Counter 진단 상태
 */
typedef enum
{
    ALIVE_STATE_NORMAL = 0,

    /* 복구 기준 Counter 수신 대기 */
    ALIVE_STATE_RECOVERY_FIRST,

    /* Counter 연속 증가 확인 */
    ALIVE_STATE_RECOVERY_CHECK

} AliveDiagState;

static uint8 prevAliveCounter = 0U;
static uint8 sequenceErrorCnt = 0U;
static uint8 recoveryCount = 0U;

static boolean firstValid = FALSE;

static AliveDiagState aliveState =
    ALIVE_STATE_NORMAL;

void ComDiag_func(void)
{
    uint8 aliveCounter = 0U;
    uint8 expectedAliveCounter = 0U;

    boolean comFault = TRUE;

    Std_ReturnType retAlive;

    /*
     * Alive Counter 수신
     */
    retAlive =
        Rte_Read_Project_SSU_SteerInfo_SSU_AliveCounter(
            &aliveCounter);

    /*
     * RTE Read 실패
     */
    if (retAlive != E_OK)
    {
        aliveState =
            ALIVE_STATE_RECOVERY_FIRST;

        sequenceErrorCnt = 0U;
        recoveryCount = 0U;
    }
    else if (firstValid == FALSE)
    {
        /*
         * 최초 수신값은 비교 기준으로 저장
         */
        prevAliveCounter = aliveCounter;
        firstValid = TRUE;

        sequenceErrorCnt = 0U;
        recoveryCount = 0U;
    }
    else
    {
        switch (aliveState)
        {
            case ALIVE_STATE_NORMAL:
            {
                expectedAliveCounter =
                    (uint8)(prevAliveCounter + 1U);

                if (aliveCounter ==
                    expectedAliveCounter)
                {
                    sequenceErrorCnt = 0U;

                    prevAliveCounter =
                        aliveCounter;
                }
                else
                {
                    if (sequenceErrorCnt <
                        SEQUENCE_ERROR_LIMIT)
                    {
                        sequenceErrorCnt++;
                    }

                    if (sequenceErrorCnt >=
                        SEQUENCE_ERROR_LIMIT)
                    {
                        /*
                         * Sequence Error 2회 연속:
                         * 통신 Fault 발생
                         */
                        aliveState =
                            ALIVE_STATE_RECOVERY_FIRST;

                        sequenceErrorCnt = 0U;
                        recoveryCount = 0U;

                        /*
                         * Fault 발생에 사용된 Counter는
                         * 복구 기준값으로 사용하지 않음
                         */
                    }
                    else
                    {
                        prevAliveCounter =
                            aliveCounter;
                    }
                }

                break;
            }

            case ALIVE_STATE_RECOVERY_FIRST:
            {
                /*
                 * Fault 이후 첫 수신값을
                 * 새로운 복구 기준값으로 저장
                 */
                prevAliveCounter = aliveCounter;

                recoveryCount = 0U;

                aliveState =
                    ALIVE_STATE_RECOVERY_CHECK;

                break;
            }

            case ALIVE_STATE_RECOVERY_CHECK:
            {
                expectedAliveCounter =
                    (uint8)(prevAliveCounter + 1U);

                if (aliveCounter ==
                    expectedAliveCounter)
                {
                    /*
                     * 연속 증가 확인
                     */
                    if (recoveryCount <
                        RECOVERY_VALID_LIMIT)
                    {
                        recoveryCount++;
                    }

                    prevAliveCounter =
                        aliveCounter;

                    /*
                     * +1 증가 3회 연속 확인
                     */
                    if (recoveryCount >=
                        RECOVERY_VALID_LIMIT)
                    {
                        aliveState =
                            ALIVE_STATE_NORMAL;

                        sequenceErrorCnt = 0U;
                        recoveryCount = 0U;
                    }
                }
                else
                {
                    /*
                     * 복구 중 Counter 불연속 발생:
                     * 다음 수신값부터 복구 재시작
                     */
                    aliveState =
                        ALIVE_STATE_RECOVERY_FIRST;

                    recoveryCount = 0U;
                }

                break;
            }

            default:
            {
                aliveState =
                    ALIVE_STATE_RECOVERY_FIRST;

                sequenceErrorCnt = 0U;
                recoveryCount = 0U;

                break;
            }
        }
    }

    /*
     * 최종 통신 진단 결과
     */
    if ((retAlive == E_OK) &&
        (firstValid == TRUE) &&
        (aliveState == ALIVE_STATE_NORMAL))
    {
        comFault = FALSE;
    }

    /*
     * SafetyPolicy에 통신 Fault 전달
     */
    (void)Rte_Write_P_ComDiagToSafetyPolicy_ComFault(
        comFault);
}
