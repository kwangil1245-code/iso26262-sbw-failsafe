
#include "Rte_SWC_MotorActuator.h"

void MotorActuator_func(void)
{
    sint16 pwmInfo = 0;

    boolean left = FALSE;
    boolean right = FALSE;
    boolean keepGo = FALSE;

    Std_ReturnType retPwm;
    Std_ReturnType retLeft;
    Std_ReturnType retRight;
    Std_ReturnType retGo;

    /*
     * MotorControl에서 제어 명령 수신
     */
    retPwm =
        Rte_Read_R_MotorControlToMotorActuator_Pwm_info(
            &pwmInfo);

    retLeft =
        Rte_Read_R_MotorControlToMotorActuator_Left(
            &left);

    retRight =
        Rte_Read_R_MotorControlToMotorActuator_Right(
            &right);

    retGo =
        Rte_Read_R_MotorControlToMotorActuator_Keep_Go(
            &keepGo);

    /*
     * RTE Read 실패 시 출력 차단
     */
    if ((retPwm != E_OK) ||
        (retLeft != E_OK) ||
        (retRight != E_OK) ||
        (retGo != E_OK))
    {
        keepGo = FALSE;
    }

    /*
     * 유효하지 않은 PWM 또는 방향 명령 차단
     */
    if ((pwmInfo < 0) ||
        (pwmInfo > 16384) ||
        (left == right))
    {
        keepGo = FALSE;
    }

    /*
     * Fail-Safe 또는 정지 명령
     */
    if (keepGo == FALSE)
    {
        (void)Rte_Call_R_PwmMotor_SetDutyCycle(0U);

        (void)Rte_Call_R_MotorIn1_WriteDirect(FALSE);
        (void)Rte_Call_R_MotorIn2_WriteDirect(FALSE);

        (void)Rte_Call_R_StopLed_WriteDirect(TRUE);
    }
    else
    {
        /*
         * 모터 방향 및 PWM 출력
         */
        (void)Rte_Call_R_MotorIn1_WriteDirect(left);
        (void)Rte_Call_R_MotorIn2_WriteDirect(right);

        (void)Rte_Call_R_PwmMotor_SetDutyCycle(
            (uint16)pwmInfo);

        (void)Rte_Call_R_StopLed_WriteDirect(FALSE);
    }
}
