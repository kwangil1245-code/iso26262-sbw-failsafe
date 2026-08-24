#include "WdgM.h"
#include "Rte_SWC_SafetyPolicy.h"

#define SWC_SafetyPolicy_START_SEC_CODE
#include "SWC_SafetyPolicy_MemMap.h"

static boolean App_IsWdgmFault(WdgM_GlobalStatusType status)
{
    if ((status == WDGM_GLOBAL_STATUS_FAILED) ||
        (status == WDGM_GLOBAL_STATUS_EXPIRED) ||
        (status == WDGM_GLOBAL_STATUS_STOPPED))
    {
        return TRUE;
    }

    return FALSE;
}

static boolean gIsFailsafe = FALSE;
static uint8 gNormalRecoverCnt = 0U;

void SafetyPolicy_PreCheck_func(void)
{
    sint16 inSteer = 0;
    boolean inFlag = FALSE;
    sint16 outSteer = 0;
    boolean outFlag = FALSE;
    boolean curFault = FALSE;
    WdgM_GlobalStatusType wdgmStatus = WDGM_GLOBAL_STATUS_OK;
    WdgM_SupervisedEntityIdType expiredSeId = 0U;

    (void)Rte_Call_SWC_SafetyPolicy_SE_SafetyPolicy_CheckpointReached(1U);

    (void)Rte_Read_R_CanMonitorToSafetyPolicy_Steer_info(&inSteer);
    (void)Rte_Read_R_CanMonitorToSafetyPolicy_Flag(&inFlag);
    (void)Rte_Call_WdgM_API_R_GetGlobalStatus(&wdgmStatus);

    if (App_IsWdgmFault(wdgmStatus) == TRUE)
    {
        curFault = TRUE;

        if (wdgmStatus == WDGM_GLOBAL_STATUS_EXPIRED)
        {
            (void)Rte_Call_WdgM_API_R_GetFirstExpiredSEID(&expiredSeId);
        }
    }

    if (inFlag == TRUE)
    {
        curFault = TRUE;
    }

    if (curFault == TRUE)
    {
        gIsFailsafe = TRUE;
        gNormalRecoverCnt = 0U;
        outFlag = TRUE;
        outSteer = 0;
    }
    else
    {
        if (gIsFailsafe == TRUE)
        {
            gNormalRecoverCnt++;

            if (gNormalRecoverCnt >= 3U)
            {
                gIsFailsafe = FALSE;
                gNormalRecoverCnt = 0U;
                outFlag = FALSE;
                outSteer = inSteer;
            }
            else
            {
                outFlag = TRUE;
                outSteer = 0;
            }
        }
        else
        {
            outFlag = FALSE;
            outSteer = inSteer;
        }
    }

    (void)Rte_Write_P_SafetyPolicyToControlCalc_Steerinfo(outSteer);
    (void)Rte_Write_P_SafetyPolicyToControlCalc_flag(outFlag);
    (void)Rte_Call_SWC_SafetyPolicy_SE_SafetyPolicy_CheckpointReached(2U);
}

#define SWC_SafetyPolicy_STOP_SEC_CODE
#include "SWC_SafetyPolicy_MemMap.h"
