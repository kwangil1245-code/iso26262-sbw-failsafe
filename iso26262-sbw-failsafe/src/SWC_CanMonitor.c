#include "Rte_SWC_CanMonitor.h"

#define STEER_ANGLE_MIN          (-512)
#define STEER_ANGLE_MAX          (511)

/* Alive Counter 불연속 허용 횟수 */
#define SEQUENCE_ERROR_LIMIT     (2U)

/* FAIL-SAFE 이후 정상 Counter 복구 확인 횟수 */
#define RECOVERY_VALID_LIMIT     (3U)

/*
 * Alive Counter 모니터링 상태
 */
typedef enum
{
    ALIVE_STATE_NORMAL = 0,

    /*
     * FAIL-SAFE 진입 후 첫 번째 정상 Counter를
     * 기다리는 상태
     */
    ALIVE_STATE_RECOVERY_FIRST,

    /*
     * 첫 번째 Counter 저장 후 연속적인
     * Counter 증가를 확인하는 상태
     */
    ALIVE_STATE_RECOVERY_CHECK
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
         * 복구 과정에서 RTE Read가 실패하면
         * 정상 복구 확인을 처음부터 다시 수행한다.
         */
        if (aliveState != ALIVE_STATE_NORMAL)
        {
            aliveState =
                ALIVE_STATE_RECOVERY_FIRST;

            recoveryCount = 0U;
        }
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
            switch (aliveState)
            {
                case ALIVE_STATE_NORMAL:
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

                    if (aliveCounter ==
                        expectedAliveCounter)
                    {
                        /*
                         * 정상적인 Counter 증가가 확인되면
                         * 연속 Sequence Error 횟수를 초기화한다.
                         */
                        sequenceErrorCnt = 0U;

                        /*
                         * 현재 Counter를 다음 주기의
                         * 비교 기준값으로 저장한다.
                         */
                        prevAliveCounter =
                            aliveCounter;
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

                        if (sequenceErrorCnt >=
                            SEQUENCE_ERROR_LIMIT)
                        {
                            /*
                             * CR-004 변경사항
                             *
                             * Sequence Error가 2회 연속
                             * 확인되면 복구 첫 수신 대기
                             * 상태로 전환한다.
                             */
                            aliveState =
                                ALIVE_STATE_RECOVERY_FIRST;

                            sequenceErrorCnt = 0U;
                            recoveryCount = 0U;

                            /*
                             * FAIL-SAFE 진입에 사용된
                             * 현재 Counter는 prevAliveCounter에
                             * 저장하지 않는다.
                             *
                             * 다음 주기에 수신되는 Counter부터
                             * 새로운 복구 시퀀스를 구성한다.
                             */
                        }
                        else
                        {
                            /*
                             * Sequence Error가 허용 횟수에
                             * 도달하지 않은 경우 현재 Counter를
                             * 다음 검사의 기준값으로 저장한다.
                             */
                            prevAliveCounter =
                                aliveCounter;
                        }
                    }

                    break;
                }

                case ALIVE_STATE_RECOVERY_FIRST:
                {
                    /*
                     * CR-004 변경사항
                     *
                     * FAIL-SAFE 진입 이후 처음 수신된
                     * Counter를 새로운 복구 시퀀스의
                     * 첫 번째 값으로 저장한다.
                     */
                    prevAliveCounter =
                        aliveCounter;

                    recoveryCount = 1U;

                    /*
                     * 첫 번째 복구 Counter가 저장되었으므로
                     * 연속 증가 확인 상태로 전환한다.
                     */
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
                         * FAIL-SAFE 진입 이후 정상 Counter가
                         * 3회 연속 확인되면 NORMAL로 복귀한다.
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
                         * CR-004 변경사항
                         *
                         * 복구 중 동일하거나 불연속적인
                         * Counter가 수신되면 해당 Counter를
                         * 정상 복구 시퀀스에 포함하지 않는다.
                         */
                        aliveState =
                            ALIVE_STATE_RECOVERY_FIRST;

                        recoveryCount = 0U;

                        /*
                         * 현재 Counter는 복구 기준값으로
                         * 저장하지 않는다.
                         *
                         * 다음 주기에 수신되는 Counter부터
                         * 새로운 복구 시퀀스를 시작한다.
                         */
                    }

                    break;
                }

                default:
                {
                    /*
                     * 정의되지 않은 상태에서는
                     * 안전하게 복구 첫 수신 대기 상태로
                     * 전환한다.
                     */
                    aliveState =
                        ALIVE_STATE_RECOVERY_FIRST;

                    sequenceErrorCnt = 0U;
                    recoveryCount = 0U;

                    break;
                }
            }
        }
    }

    /*
     * NORMAL 상태가 아니면 Alive Counter Fault가
     * 유지되고 있는 것으로 판단한다.
     */
    if ((aliveState != ALIVE_STATE_NORMAL) ||
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
