
#include "Rte_SWC_SafetyManager.h"

#define SWC_SafetyManager_START_SEC_CODE
#include "SWC_SafetyManager_MemMap.h"

void SafetyManager_func(void)
{
    boolean comFault = TRUE;
    boolean angleFault = TRUE;
    boolean execFault = TRUE;

    boolean outFlag = TRUE;

    sint16 inSteer = 0;
    sint16 outSteer = 0;

    Std_ReturnType retCom;
    Std_ReturnType retAngle;
    Std_ReturnType retExec;
    Std_ReturnType retSteer;

    /*
     * WdgM Checkpoint 1
     */
    (void)Rte_Call_SWC_SafetyManager_SE_SafetyManager_CheckpointReached(1U);

    /*
     * 각 Diag SWC의 진단 결과 수신
     */
    retCom =
        Rte_Read_R_ComDiagToSafetyManager_ComFault(
            &comFault);

    retAngle =
        Rte_Read_R_AngleDiagToSafetyManager_AngleFault(
            &angleFault);

    retExec =
        Rte_Read_R_ExecDiagToSafetyManager_ExecFault(
            &execFault);

    retSteer =
        Rte_Read_R_AngleDiagToSafetyManager_SteerAngle(
            &inSteer);

    /*
     * 하나라도 Fault이면 FAIL-SAFE
     *
     * RTE Read 실패도 Fault로 처리
     */
    if ((retCom != E_OK) ||
        (retAngle != E_OK) ||
        (retExec != E_OK) ||
        (retSteer != E_OK) ||
        (comFault == TRUE) ||
        (angleFault == TRUE) ||
        (execFault == TRUE))
    {
        outFlag = TRUE;
        outSteer = 0;
    }
    else
    {
        outFlag = FALSE;
        outSteer = inSteer;
    }

    /*
     * ControlCalc에 안전 출력 전달
     */
    (void)Rte_Write_P_SafetyManagerToControlCalc_Steerinfo(
        outSteer);

    (void)Rte_Write_P_SafetyManagerToControlCalc_flag(
        outFlag);

    /*
     * WdgM Checkpoint 2
     */
    (void)Rte_Call_SWC_SafetyManager_SE_SafetyManager_CheckpointReached(2U);
}

#define SWC_SafetyManager_STOP_SEC_CODE
#include "SWC_SafetyManager_MemMap.h"
