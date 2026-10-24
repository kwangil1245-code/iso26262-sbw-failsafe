
#include "WdgM.h"
#include "Rte_SWC_ExecDiag.h"

static boolean App_IsWdgmFault(
    WdgM_GlobalStatusType status)
{
    if ((status == WDGM_GLOBAL_STATUS_FAILED) ||
        (status == WDGM_GLOBAL_STATUS_EXPIRED) ||
        (status == WDGM_GLOBAL_STATUS_STOPPED))
    {
        return TRUE;
    }

    return FALSE;
}

void ExecDiag_func(void)
{
    boolean execFault = TRUE;

    WdgM_GlobalStatusType wdgmStatus =
        WDGM_GLOBAL_STATUS_OK;

    WdgM_SupervisedEntityIdType expiredSeId = 0U;

    Std_ReturnType retStatus;

    /*
     * WdgM Global Status 확인
     */
    retStatus =
        Rte_Call_WdgM_API_R_GetGlobalStatus(
            &wdgmStatus);

    if (retStatus == E_OK)
    {
        execFault = App_IsWdgmFault(wdgmStatus);

        if (wdgmStatus == WDGM_GLOBAL_STATUS_EXPIRED)
        {
            (void)Rte_Call_WdgM_API_R_GetFirstExpiredSEID(
                &expiredSeId);
        }
    }

    /*
     * SafetyPolicy에 SW 실행 이상 전달
     */
    (void)Rte_Write_P_ExecDiagToSafetyPolicy_ExecFault(
        execFault);
}
