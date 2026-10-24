
#include "Rte_SWC_MotorActuator.h"

#define PWM_DUTY_MAX (16384U)

void MotorActuator_func(void)
{
    uint16 pwmDutyCycle = 0U;

    boolean motorDirectionLeft = FALSE;
    boolean motorDirectionRight = FALSE;
    boolean motorEnable = FALSE;

    Std_ReturnType retPwm;
    Std_ReturnType retLeft;
    Std_ReturnType retRight;
    Std_ReturnType retEnable;

    retPwm =
        Rte_Read_R_MotorDriveCommand_PwmDutyCycle(
            &pwmDutyCycle);

    retLeft =
        Rte_Read_R_MotorDriveCommand_MotorDirectionLeft(
            &motorDirectionLeft);

    retRight =
        Rte_Read_R_MotorDriveCommand_MotorDirectionRight(
            &motorDirectionRight);

    retEnable =
        Rte_Read_R_MotorDriveCommand_MotorEnable(
            &motorEnable);

    if ((retPwm != E_OK) ||
        (retLeft != E_OK) ||
        (retRight != E_OK) ||
        (retEnable != E_OK))
    {
        motorEnable = FALSE;
    }

    if ((pwmDutyCycle > PWM_DUTY_MAX) ||
        (motorDirectionLeft == motorDirectionRight))
    {
        motorEnable = FALSE;
    }

    if (motorEnable == FALSE)
    {
        (void)Rte_Call_R_PwmMotor_SetDutyCycle(0U);
        (void)Rte_Call_R_MotorIn1_WriteDirect(FALSE);
        (void)Rte_Call_R_MotorIn2_WriteDirect(FALSE);
        (void)Rte_Call_R_StopLed_WriteDirect(TRUE);
    }
    else
    {
        (void)Rte_Call_R_MotorIn1_WriteDirect(
            motorDirectionLeft);

        (void)Rte_Call_R_MotorIn2_WriteDirect(
            motorDirectionRight);

        (void)Rte_Call_R_PwmMotor_SetDutyCycle(
            pwmDutyCycle);

        (void)Rte_Call_R_StopLed_WriteDirect(FALSE);
    }
}
