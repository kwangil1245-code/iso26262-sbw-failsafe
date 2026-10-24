
#include "WdgM.h"
#include "Rte_SWC_ExecDiag.h"

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

void ExecDiag_func(void)
{
    boolean executionFault = TRUE;

    WdgM_GlobalStatusType wdgmStatus =
        WDGM_GLOBAL_STATUS_OK;

    WdgM_SupervisedEntityIdType expiredSeId = 0U;
    Std_ReturnType retStatus;

    retStatus = Rte_Call_R_WdgMService_GetGlobalStatus(
        &wdgmStatus);

    if (retStatus == E_OK)
    {
        executionFault = App_IsWdgmFault(wdgmStatus);

        if (wdgmStatus == WDGM_GLOBAL_STATUS_EXPIRED)
        {
            (void)Rte_Call_R_WdgMService_GetFirstExpiredSEID(
                &expiredSeId);
        }
    }

    (void)Rte_Write_P_ExecutionDiagnostic_ExecutionFault(
        executionFault);
}
