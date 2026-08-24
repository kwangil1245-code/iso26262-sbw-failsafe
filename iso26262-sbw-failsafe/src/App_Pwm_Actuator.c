#include "Rte_SWC_Pwm_Actuator.h"

void Pwm_Actuator_func(void)
{
    sint16 Pwm_info = 0;
    boolean Left = TRUE;
    boolean Right = FALSE;
    boolean Keep_Go = TRUE;

    (void)Rte_Read_R_ControlCalcToActuator_Pwm_info(&Pwm_info);
    (void)Rte_Read_R_ControlCalcToActuator_Left(&Left);
    (void)Rte_Read_R_ControlCalcToActuator_Right(&Right);
    (void)Rte_Read_R_ControlCalcToActuator_Keep_Go(&Keep_Go);

    if (Keep_Go == FALSE)
    {
        (void)Rte_Call_R_PwmMotor_SetDutyCycle(0U);
        (void)Rte_Call_R_MotorIn1_WriteDirect(FALSE);
        (void)Rte_Call_R_MotorIn2_WriteDirect(FALSE);
        (void)Rte_Call_R_StopLed_WriteDirect(TRUE);
    }
    else
    {
        (void)Rte_Call_R_MotorIn1_WriteDirect(Left);
        (void)Rte_Call_R_MotorIn2_WriteDirect(Right);
        (void)Rte_Call_R_PwmMotor_SetDutyCycle((uint16)Pwm_info);
        (void)Rte_Call_R_StopLed_WriteDirect(FALSE);
    }
}
