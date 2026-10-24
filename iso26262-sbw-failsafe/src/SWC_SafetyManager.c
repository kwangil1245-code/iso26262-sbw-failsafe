
#include "Rte_SWC_SafetyManager.h"

#define SWC_SafetyManager_START_SEC_CODE
#include "SWC_SafetyManager_MemMap.h"

void SafetyManager_func(void)
{
    boolean communicationFault = TRUE;
    boolean steeringAngleFault = TRUE;
    boolean executionFault = TRUE;
    boolean failSafeActive = TRUE;

    sint16 validatedSteeringAngle = 0;
    sint16 steeringAngleCommand = 0;

    Std_ReturnType retCom;
    Std_ReturnType retAngleFault;
    Std_ReturnType retExec;
    Std_ReturnType retSteeringAngle;

    (void)Rte_Call_R_SafetyWdgMCheckpoint_CheckpointReached(1U);

    retCom =
        Rte_Read_R_CommunicationDiagnostic_CommunicationFault(
            &communicationFault);

    retAngleFault =
        Rte_Read_R_SteeringAngleDiagnostic_SteeringAngleFault(
            &steeringAngleFault);

    retSteeringAngle =
        Rte_Read_R_SteeringAngleDiagnostic_ValidatedSteeringAngle(
            &validatedSteeringAngle);

    retExec =
        Rte_Read_R_ExecutionDiagnostic_ExecutionFault(
            &executionFault);

    if ((retCom == E_OK) &&
        (retAngleFault == E_OK) &&
        (retExec == E_OK) &&
        (retSteeringAngle == E_OK) &&
        (communicationFault == FALSE) &&
        (steeringAngleFault == FALSE) &&
        (executionFault == FALSE))
    {
        failSafeActive = FALSE;
        steeringAngleCommand = validatedSteeringAngle;
    }
    else
    {
        failSafeActive = TRUE;
        steeringAngleCommand = 0;
    }

    (void)Rte_Write_P_SafeSteeringCommand_SteeringAngleCommand(
        steeringAngleCommand);

    (void)Rte_Write_P_SafeSteeringCommand_FailSafeActive(
        failSafeActive);

    (void)Rte_Call_R_SafetyWdgMCheckpoint_CheckpointReached(2U);
}

#define SWC_SafetyManager_STOP_SEC_CODE
#include "SWC_SafetyManager_MemMap.h"
