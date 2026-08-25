#include "Rte_SWC_CanMonitor.h"

static uint8 prevAliveCounter = 0U;
static boolean firstValid = FALSE;
static uint8 sameCounterCnt = 0U;

void CanMonitor_func(void)
{
    uint8 aliveCounter = 0U;
    sint16 angle = 0;
    boolean flag = FALSE;

    Std_ReturnType retAlive;
    Std_ReturnType retAngle;

    retAlive = Rte_Read_Project_SSU_SteerInfo_SSU_AliveCounter(&aliveCounter);
    retAngle = Rte_Read_Project_SSU_SteerInfo_SSU_SteerAngle(&angle);

    /* RTE read 실패 시 에러 */
    if ((retAlive != E_OK) || (retAngle != E_OK))
    {
        flag = TRUE;
    }
    /* 조향각 범위 이상 */
    else if ((angle < -512) || (angle > 511))
    {
        flag = TRUE;
    }
    else
    {
        /* 첫 수신은 비교하지 않고 기준값만 저장 */
        if (firstValid == FALSE)
        {
            prevAliveCounter = aliveCounter;
            firstValid = TRUE;
            sameCounterCnt = 0U;
            flag = FALSE;
        }
        else
        {
            /* 같은 alive가 연속으로 들어오면 에러 후보 */
            if (aliveCounter == prevAliveCounter)
            {
                sameCounterCnt++;
                if (sameCounterCnt >= 2U)
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
                sameCounterCnt = 0U;
                flag = FALSE;
            }

            prevAliveCounter = aliveCounter;
        }
    }

    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Steer_info(angle);
    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Flag(flag);
}
 