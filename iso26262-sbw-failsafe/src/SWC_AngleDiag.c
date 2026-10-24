
#include "Rte_SWC_AngleDiag.h"

#define STEERING_ANGLE_MIN (-512)
#define STEERING_ANGLE_MAX (511)

void AngleDiag_func(void)
{
    sint16 steeringAngle = 0;
    sint16 validatedSteeringAngle = 0;

    boolean steeringAngleFault = TRUE;
    Std_ReturnType retAngle;

    retAngle = Rte_Read_R_SteeringSensorData_SteeringAngle(
        &steeringAngle);

    if ((retAngle == E_OK) &&
        (steeringAngle >= STEERING_ANGLE_MIN) &&
        (steeringAngle <= STEERING_ANGLE_MAX))
    {
        steeringAngleFault = FALSE;
        validatedSteeringAngle = steeringAngle;
    }

    (void)Rte_Write_P_SteeringAngleDiagnostic_SteeringAngleFault(
        steeringAngleFault);

    (void)Rte_Write_P_SteeringAngleDiagnostic_ValidatedSteeringAngle(
        validatedSteeringAngle);
}
