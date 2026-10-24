
#include "Rte_SWC_AngleDiag.h"

#define STEER_ANGLE_MIN    (-512)
#define STEER_ANGLE_MAX    (511)

/*
 * 조향각 유효성 진단
 */
void AngleDiag_func(void)
{
    sint16 angle = 0;

    boolean angleFault = TRUE;

    Std_ReturnType retAngle;

    /*
     * RTE를 통해 조향각 수신
     */
    retAngle =
        Rte_Read_Project_SSU_SteerInfo_SSU_SteerAngle(
            &angle);

    /*
     * 조향각 수신 성공 여부 확인
     */
    if (retAngle == E_OK)
    {
        /*
         * 조향각 유효 범위 검사
         */
        if ((angle >= STEER_ANGLE_MIN) &&
            (angle <= STEER_ANGLE_MAX))
        {
            angleFault = FALSE;
        }
    }

    /*
     * 진단 결과 전달
     */
    (void)Rte_Write_P_AngleDiagToSafetyPolicy_AngleFault(
        angleFault);

    /*
     * 조향각 데이터 전달
     *
     * 유효하지 않은 경우 0을 전달하고,
     * SafetyPolicy에서 AngleFault를 확인하여
     * 제어에 사용하지 않도록 한다.
     */
    if (angleFault == TRUE)
    {
        angle = 0;
    }

    (void)Rte_Write_P_AngleDiagToSafetyPolicy_SteerAngle(
        angle);
}
