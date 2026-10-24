
#include "Rte_SWC_MotorControl.h"

#define PWM_MAX              (32768U)
#define STEER_DUTY           (16384U)
#define STEER_DIFF_THRESHOLD (2)

static sint16 prevInputSteer = 0;
static boolean firstNormalInput = FALSE;

void MotorControl_func(void)
{
    sint16 inputSteer = 0;
    sint16 steerDiff = 0;
    sint16 absDiff = 0;

    uint16 pwmDuty = 0U;
    uint32 relativeDuty = 0U;

    boolean inputFault = TRUE;
    boolean left = FALSE;
    boolean right = FALSE;
    boolean keepGo = FALSE;

    Std_ReturnType retSteer;
    Std_ReturnType retFlag;

    /*
     * SafetyManager 출력 수신
     */
    retSteer =
        Rte_Read_R_SafetyManagerToMotorControl_Steerinfo(
            &inputSteer);

    retFlag =
        Rte_Read_R_SafetyManagerToMotorControl_flag(
            &inputFault);

    /*
     * RTE Read 실패 또는 Fail-Safe
     */
    if ((retSteer != E_OK) ||
        (retFlag != E_OK) ||
        (inputFault == TRUE))
    {
        /*
         * 모터 구동 차단
         * 복귀 시 기준값 재설정
         */
        firstNormalInput = FALSE;
    }
    else if (firstNormalInput == FALSE)
    {
        /*
         * Fail-Safe 해제 이후 첫 정상값은
         * 비교 기준으로만 저장
         */
        prevInputSteer = inputSteer;
        firstNormalInput = TRUE;
    }
    else
    {
        steerDiff = inputSteer - prevInputSteer;

        if (steerDiff < 0)
        {
            absDiff = (sint16)(-steerDiff);
        }
        else
        {
            absDiff = steerDiff;
        }

        /*
         * 조향값 변화 방향 판단
         */
        if (steerDiff > STEER_DIFF_THRESHOLD)
        {
            left = FALSE;
            right = TRUE;
            keepGo = TRUE;
        }
        else if (steerDiff < -STEER_DIFF_THRESHOLD)
        {
            left = TRUE;
            right = FALSE;
            keepGo = TRUE;
        }

        /*
         * 조향값 변화량에 따른 PWM 계산
         */
        if (keepGo == TRUE)
        {
            if (absDiff > 512)
            {
                absDiff = 512;
            }

            relativeDuty =
                ((uint32)absDiff * PWM_MAX) / 512U;

            pwmDuty = (uint16)(
                ((uint32)STEER_DUTY * relativeDuty)
                >> 15
            );
        }

        prevInputSteer = inputSteer;
    }

    /*
     * MotorActuator에 제어 명령 전달
     */
    (void)Rte_Write_P_MotorControlToMotorActuator_Pwm_info(
        (sint16)pwmDuty);

    (void)Rte_Write_P_MotorControlToMotorActuator_Left(
        left);

    (void)Rte_Write_P_MotorControlToMotorActuator_Right(
        right);

    (void)Rte_Write_P_MotorControlToMotorActuator_Keep_Go(
        keepGo);
}
