#include "Rte_SWC_CanMonitor.h"

#define SEQUENCE_ERROR_LIMIT    (2U)

static uint8 prevAliveCounter = 0U;
static boolean firstValid = FALSE;
static uint8 sequenceErrorCnt = 0U;

void CanMonitor_func(void)
{
    uint8 aliveCounter = 0U;
    uint8 expectedAliveCounter = 0U;
    sint16 angle = 0;
    boolean flag = FALSE;

    Std_ReturnType retAlive;
    Std_ReturnType retAngle;

    retAlive =
        Rte_Read_Project_SSU_SteerInfo_SSU_AliveCounter(&aliveCounter);

    retAngle =
        Rte_Read_Project_SSU_SteerInfo_SSU_SteerAngle(&angle);

    /* RTE 데이터 수신 실패 */
    if ((retAlive != E_OK) || (retAngle != E_OK))
    {
        flag = TRUE;
    }
    /* 조향각 유효 범위 이탈 */
    else if ((angle < -512) || (angle > 511))
    {
        flag = TRUE;
    }
    else
    {
        /* 첫 수신값은 비교하지 않고 기준값으로 저장한다. */
        if (firstValid == FALSE)
        {
            prevAliveCounter = aliveCounter;
            firstValid = TRUE;
            sequenceErrorCnt = 0U;
            flag = FALSE;
        }
        else
        {
            /*
             * 다음 Alive Counter 예상값을 계산한다.
             * uint8이므로 255 다음에는 0으로 순환한다.
             */
            expectedAliveCounter =
                (uint8)(prevAliveCounter + 1U);

            if (aliveCounter != expectedAliveCounter)
            {
                /* 불연속 증가가 검출되면 오류 횟수를 증가시킨다. */
                sequenceErrorCnt++;

                /*
                 * 불연속 증가가 2회 연속 검출되면
                 * Alive Counter Sequence Fault로 판단한다.
                 */
                if (sequenceErrorCnt >= SEQUENCE_ERROR_LIMIT)
                {
                    flag = TRUE;
                }
                else
                {
                    flag = FALSE;
                }
            }
            else
            {
                /* 정상 증가가 확인되면 연속 오류 횟수를 초기화한다. */
                sequenceErrorCnt = 0U;
                flag = FALSE;
            }

            /* 현재 값을 다음 수신 주기의 비교 기준값으로 저장한다. */
            prevAliveCounter = aliveCounter;
        }
    }

    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Steer_info(angle);
    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Flag(flag);
}
