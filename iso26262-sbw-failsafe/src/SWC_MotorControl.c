
#include "Rte_SWC_MotorControl.h"

#define PWM_SCALE_MAX        (32768U)
#define PWM_DUTY_MAX         (16384U)
#define STEER_DIFF_MAX       (512)
#define STEER_DIFF_THRESHOLD (2)

static sint16 prevSteeringAngle = 0;
static boolean referenceInitialized = FALSE;

void MotorControl_func(void)
{
    sint16 steeringAngleCommand = 0;
    sint32 steeringDiff = 0;
    sint32 absDiff = 0;

    uint16 pwmDutyCycle = 0U;
    uint32 relativeDutyCycle = 0U;

    boolean failSafeActive = TRUE;
    boolean motorDirectionLeft = FALSE;
    boolean motorDirectionRight = FALSE;
    boolean motorEnable = FALSE;

    Std_ReturnType retAngle;
    Std_ReturnType retFailSafe;

    retAngle =
        Rte_Read_R_SafeSteeringCommand_SteeringAngleCommand(
            &steeringAngleCommand);

    retFailSafe =
        Rte_Read_R_SafeSteeringCommand_FailSafeActive(
            &failSafeActive);

    if ((retAngle != E_OK) ||
        (retFailSafe != E_OK) ||
        (failSafeActive == TRUE))
    {
        referenceInitialized = FALSE;
    }
    else if (referenceInitialized == FALSE)
    {
        prevSteeringAngle = steeringAngleCommand;
        referenceInitialized = TRUE;
    }
    else
    {
        steeringDiff =
            (sint32)steeringAngleCommand -
            (sint32)prevSteeringAngle;

        if (steeringDiff < 0)
        {
            absDiff = -steeringDiff;
        }
        else
        {
            absDiff = steeringDiff;
        }

        if (steeringDiff > STEER_DIFF_THRESHOLD)
        {
            motorDirectionLeft = FALSE;
            motorDirectionRight = TRUE;
            motorEnable = TRUE;
        }
        else if (steeringDiff < -STEER_DIFF_THRESHOLD)
        {
            motorDirectionLeft = TRUE;
            motorDirectionRight = FALSE;
            motorEnable = TRUE;
        }

        if (motorEnable == TRUE)
        {
            if (absDiff > STEER_DIFF_MAX)
            {
                absDiff = STEER_DIFF_MAX;
            }

            relativeDutyCycle =
                ((uint32)absDiff * PWM_SCALE_MAX) /
                (uint32)STEER_DIFF_MAX;

            pwmDutyCycle = (uint16)(
                ((uint32)PWM_DUTY_MAX * relativeDutyCycle) >> 15
            );
        }

        prevSteeringAngle = steeringAngleCommand;
    }

    (void)Rte_Write_P_MotorDriveCommand_PwmDutyCycle(
        pwmDutyCycle);

    (void)Rte_Write_P_MotorDriveCommand_MotorDirectionLeft(
        motorDirectionLeft);

    (void)Rte_Write_P_MotorDriveCommand_MotorDirectionRight(
        motorDirectionRight);

    (void)Rte_Write_P_MotorDriveCommand_MotorEnable(
        motorEnable);
}
