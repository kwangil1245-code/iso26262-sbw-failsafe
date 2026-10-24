
#include "Rte_SWC_ComDiag.h"

#define SEQUENCE_ERROR_LIMIT  (2U)
#define RECOVERY_VALID_LIMIT  (3U)

/* Alive Counter 진단 상태 */
typedef enum
{
    ALIVE_STATE_INIT = 0,
    ALIVE_STATE_NORMAL,
    ALIVE_STATE_RECOVERY_FIRST,
    ALIVE_STATE_RECOVERY_CHECK

} AliveDiagState;

static AliveDiagState aliveState = ALIVE_STATE_INIT;

static uint8 prevAliveCounter = 0U;
static uint8 sequenceErrorCnt = 0U;
static uint8 recoveryCount = 0U;

void ComDiag_func(void)
{
    uint8 aliveCounter = 0U;
    uint8 expectedAliveCounter = 0U;

    boolean comFault = TRUE;
    Std_ReturnType retAlive;

    /* Alive Counter 수신 */
    retAlive =
        Rte_Read_Project_SSU_SteerInfo_SSU_AliveCounter(
            &aliveCounter);

    if (retAlive != E_OK)
    {
        /*
         * 수신 실패 시 FAIL-SAFE 진단 상태 진입
         * 다음 정상 수신부터 복구 검사 시작
         */
        aliveState = ALIVE_STATE_RECOVERY_FIRST;

        sequenceErrorCnt = 0U;
        recoveryCount = 0U;
    }
    else
    {
        switch (aliveState)
        {
            case ALIVE_STATE_INIT:
            {
                /*
                 * 최초 수신값을 기준으로 저장
                 * 최초 수신만으로 NORMAL 허용하지 않음
                 */
                prevAliveCounter = aliveCounter;

                sequenceErrorCnt = 0U;
                recoveryCount = 0U;

                aliveState = ALIVE_STATE_RECOVERY_CHECK;

                break;
            }

            case ALIVE_STATE_NORMAL:
            {
                expectedAliveCounter =
                    (uint8)(prevAliveCounter + 1U);

                if (aliveCounter == expectedAliveCounter)
                {
                    sequenceErrorCnt = 0U;
                    prevAliveCounter = aliveCounter;
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
                         * 복구 상태로 전환
                         */
                        aliveState =
                            ALIVE_STATE_RECOVERY_FIRST;

                        sequenceErrorCnt = 0U;
                        recoveryCount = 0U;

                        /*
                         * Fault 발생 Counter는
                         * 복구 기준값으로 사용하지 않음
                         */
                    }
                    else
                    {
                        prevAliveCounter = aliveCounter;
                    }
                }

                break;
            }

            case ALIVE_STATE_RECOVERY_FIRST:
            {
                /*
                 * Fault 이후 첫 수신값을
                 * 복구 기준값으로 저장
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

                if (aliveCounter == expectedAliveCounter)
                {
                    recoveryCount++;
                    prevAliveCounter = aliveCounter;

                    /*
                     * +1 증가 3회 연속 확인
                     */
                    if (recoveryCount >=
                        RECOVERY_VALID_LIMIT)
                    {
                        aliveState = ALIVE_STATE_NORMAL;

                        sequenceErrorCnt = 0U;
                        recoveryCount = 0U;
                    }
                }
                else
                {
                    /*
                     * 복구 중 불연속 발생
                     * 현재 값은 기준으로 사용하지 않음
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

    /* 통신 진단 결과 */
    if ((retAlive == E_OK) &&
        (aliveState == ALIVE_STATE_NORMAL))
    {
        comFault = FALSE;
    }

    /* SafetyManager에 결과 전달 */
    (void)Rte_Write_P_ComDiagToSafetyManager_ComFault(
        comFault);
}
