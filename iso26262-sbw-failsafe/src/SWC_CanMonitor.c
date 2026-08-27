#include "Rte_SWC_CanMonitor.h"

#define STEER_ANGLE_MIN          (-512)
#define STEER_ANGLE_MAX          (511)

/* Alive Counter 불연속 허용 횟수 */
#define SEQUENCE_ERROR_LIMIT     (2U)

/* Alive Counter 정상 복구 확인 횟수 */
#define RECOVERY_VALID_LIMIT     (3U)

/*
 * Alive Counter 모니터링 상태
 */
typedef enum
{
    ALIVE_STATE_NORMAL = 0,
    ALIVE_STATE_RECOVERY
} AliveMonitorState;

static uint8 prevAliveCounter = 0U;
static uint8 sequenceErrorCnt = 0U;
static uint8 recoveryCount = 0U;

static boolean firstValid = FALSE;

static AliveMonitorState aliveState =
    ALIVE_STATE_NORMAL;

void CanMonitor_func(void)
{
    uint8 aliveCounter = 0U;
    uint8 expectedAliveCounter = 0U;

    sint16 angle = 0;

    boolean flag = FALSE;
    boolean rteFault = FALSE;
    boolean angleFault = FALSE;

    Std_ReturnType retAlive;
    Std_ReturnType retAngle;

    retAlive =
        Rte_Read_Project_SSU_SteerInfo_SSU_AliveCounter(
            &aliveCounter);

    retAngle =
        Rte_Read_Project_SSU_SteerInfo_SSU_SteerAngle(
            &angle);

    /*
     * RTE를 통해 Alive Counter 또는
     * 조향값을 수신하지 못한 경우
     */
    if ((retAlive != E_OK) ||
        (retAngle != E_OK))
    {
        rteFault = TRUE;

        /*
         * 유효한 Counter를 확인할 수 없으므로
         * 진행 중인 정상 복구 확인 횟수를 초기화한다.
         */
        recoveryCount = 0U;
    }
    else
    {
        /*
         * 조향값이 정의된 유효 범위를
         * 벗어났는지 확인한다.
         */
        if ((angle < STEER_ANGLE_MIN) ||
            (angle > STEER_ANGLE_MAX))
        {
            angleFault = TRUE;
        }

        /*
         * 최초 수신된 Alive Counter는 비교하지 않고
         * 다음 Counter 검사의 기준값으로 저장한다.
         */
        if (firstValid == FALSE)
        {
            prevAliveCounter = aliveCounter;
            firstValid = TRUE;

            sequenceErrorCnt = 0U;
            recoveryCount = 0U;
        }
        else
        {
            /*
             * 이전 Counter를 기준으로
             * 다음 예상 Counter를 계산한다.
             *
             * 자료형이 uint8이므로
             * 255 다음에는 0으로 순환한다.
             */
            expectedAliveCounter =
                (uint8)(prevAliveCounter + 1U);

            switch (aliveState)
            {
                case ALIVE_STATE_NORMAL:
                {
                    /*
                     * 현재 Counter가 예상값과
                     * 일치하는지 확인한다.
                     */
                    if (aliveCounter ==
                        expectedAliveCounter)
                    {
                        /*
                         * 정상적인 Counter 증가가 확인되면
                         * 연속 Sequence Error 횟수를 초기화한다.
                         */
                        sequenceErrorCnt = 0U;
                    }
                    else
                    {
                        /*
                         * 동일하거나 불연속적인 Counter가
                         * 수신되면 오류 횟수를 증가시킨다.
                         */
                        if (sequenceErrorCnt <
                            SEQUENCE_ERROR_LIMIT)
                        {
                            sequenceErrorCnt++;
                        }

                        /*
                         * Sequence Error가 설정된 횟수만큼
                         * 연속 확인되면 복구 확인 상태로 전환한다.
                         */
                        if (sequenceErrorCnt >=
                            SEQUENCE_ERROR_LIMIT)
                        {
                            aliveState =
                                ALIVE_STATE_RECOVERY;

                            sequenceErrorCnt = 0U;

                            /*
                             * 현재 Counter를 정상 복구 확인의
                             * 첫 번째 값으로 설정한다.
                             */
                            recoveryCount = 1U;
                        }
                    }

                    /*
                     * 현재 Counter를 다음 실행 주기의
                     * 비교 기준값으로 저장한다.
                     */
                    prevAliveCounter = aliveCounter;

                    break;
                }

                case ALIVE_STATE_RECOVERY:
                {
                    /*
                     * 복구 상태에서 현재 Counter가
                     * 예상값과 일치하는지 확인한다.
                     */
                    if (aliveCounter ==
                        expectedAliveCounter)
                    {
                        /*
                         * 정상적인 Counter 증가가 확인되면
                         * 정상 복구 확인 횟수를 증가시킨다.
                         */
                        if (recoveryCount <
                            RECOVERY_VALID_LIMIT)
                        {
                            recoveryCount++;
                        }

                        /*
                         * 현재 Counter를 다음 복구 검사의
                         * 비교 기준값으로 저장한다.
                         */
                        prevAliveCounter =
                            aliveCounter;

                        /*
                         * 정상 Counter가 설정된 횟수만큼
                         * 확인되면 정상 상태로 전환한다.
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
                         * 복구 확인 중 동일하거나 불연속적인
                         * Counter가 수신되면 현재 Counter부터
                         * 정상 복구 확인을 다시 시작한다.
                         */
                        prevAliveCounter =
                            aliveCounter;

                        recoveryCount = 1U;
                    }

                    break;
                }

                default:
                {
                    /*
                     * 정의되지 않은 상태에서는
                     * 정상 상태로 초기화한다.
                     */
                    aliveState =
                        ALIVE_STATE_NORMAL;

                    prevAliveCounter =
                        aliveCounter;

                    sequenceErrorCnt = 0U;
                    recoveryCount = 0U;

                    break;
                }
            }
        }
    }

    /*
     * 복구 확인 상태, RTE 수신 실패 또는
     * 조향값 범위 이탈 중 하나라도 발생하면
     * SafetyPolicy에 전달할 Flag를 활성화한다.
     */
    if ((aliveState == ALIVE_STATE_RECOVERY) ||
        (rteFault == TRUE) ||
        (angleFault == TRUE))
    {
        flag = TRUE;
    }
    else
    {
        flag = FALSE;
    }

    /*
     * 수신한 조향값을 SafetyPolicy에 전달한다.
     */
    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Steer_info(
        angle);

    /*
     * 종합 Fault 상태를 SafetyPolicy에 전달한다.
     */
    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Flag(
        flag);
}
