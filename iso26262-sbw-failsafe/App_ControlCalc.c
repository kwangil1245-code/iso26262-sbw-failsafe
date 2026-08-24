#include "Rte_SWC_ControlCalc.h"

#define PWM_MAX                 (32768U)
#define PWM_MIN                 (0U)
#define STEER_DUTY              (16384U)
#define STEER_DIFF_THRESHOLD    (2)

void ControlCalc_func(void)
{
    static sint16 prev_input_steer = 0;

    sint16 input_steer = 0;
    boolean input_flag = FALSE;
    sint16 steer_diff = 0;
    sint16 abs_diff = 0;
    uint16 RelativeDutyCycle = 0U;
    uint32 AbsoluteDutyCycle_tmp = 0U;
    sint16 AbsoluteDutyCycle = 0;
    boolean Left = FALSE;
    boolean Right = FALSE;
    boolean go_flag = TRUE;

    (void)Rte_Read_R_SafetyPolicyToControlCalc_Steerinfo(&input_steer);
    (void)Rte_Read_R_SafetyPolicyToControlCalc_flag(&input_flag);

    if (input_flag == TRUE)
    {
        AbsoluteDutyCycle = 0;
        go_flag = FALSE;
        Left = FALSE;
        Right = FALSE;
    }
    else
    {
        steer_diff = input_steer - prev_input_steer;

        if (steer_diff < 0)
        {
            abs_diff = (sint16)(-steer_diff);
        }
        else
        {
            abs_diff = steer_diff;
        }

        if (steer_diff > STEER_DIFF_THRESHOLD)
        {
            Left = FALSE;
            Right = TRUE;
            go_flag = TRUE;
        }
        else if (steer_diff < -STEER_DIFF_THRESHOLD)
        {
            Left = TRUE;
            Right = FALSE;
            go_flag = TRUE;
        }
        else
        {
            Left = FALSE;
            Right = FALSE;
            go_flag = FALSE;
        }

        if (abs_diff > 512)
        {
            abs_diff = 512;
        }

        RelativeDutyCycle = (uint16)((((uint32)abs_diff) * 32768U) / 512U);
        AbsoluteDutyCycle_tmp = (((uint32)STEER_DUTY * (uint32)RelativeDutyCycle) >> 15);

        if (AbsoluteDutyCycle_tmp > PWM_MAX)
        {
            AbsoluteDutyCycle = (sint16)PWM_MAX;
        }
        else
        {
            AbsoluteDutyCycle = (sint16)AbsoluteDutyCycle_tmp;
        }

        if (go_flag == FALSE)
        {
            AbsoluteDutyCycle = PWM_MIN;
        }
    }

    prev_input_steer = input_steer;

    (void)Rte_Write_P_ControlCalcToActuator_Pwm_info(AbsoluteDutyCycle);
    (void)Rte_Write_P_ControlCalcToActuator_Left(Left);
    (void)Rte_Write_P_ControlCalcToActuator_Right(Right);
    (void)Rte_Write_P_ControlCalcToActuator_Keep_Go(go_flag);
}
