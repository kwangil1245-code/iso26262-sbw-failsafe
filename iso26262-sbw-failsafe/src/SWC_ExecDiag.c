
#include "WdgM.h"
#include "Rte_SWC_ExecDiag.h"

/*
 * WdgM 상태를 Fault로 변환
 */
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
     * WdgM Global Status 조회
     */
    retStatus =
        Rte_Call_WdgM_API_R_GetGlobalStatus(
            &wdgmStatus);

    if (retStatus == E_OK)
    {
        execFault = App_IsWdgmFault(wdgmStatus);

        /*
         * EXPIRED 발생 시 원인 SE 조회
         */
        if (wdgmStatus == WDGM_GLOBAL_STATUS_EXPIRED)
        {
            (void)Rte_Call_WdgM_API_R_GetFirstExpiredSEID(
                &expiredSeId);
        }
    }

    /*
     * SafetyManager에 SW 실행 진단 결과 전달
     */
    (void)Rte_Write_P_ExecDiagToSafetyManager_ExecFault(
        execFault);
}
