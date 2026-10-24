
#include "Rte_SWC_AngleDiag.h"

#define STEER_ANGLE_MIN  (-512)
#define STEER_ANGLE_MAX  (511)

void AngleDiag_func(void)
{
    sint16 angle = 0;

    boolean angleFault = TRUE;

    Std_ReturnType retAngle;

    /* 조향각 수신 */
    retAngle =
        Rte_Read_Project_SSU_SteerInfo_SSU_SteerAngle(
            &angle);

    /*
     * 수신 성공 및 유효 범위 확인
     */
    if ((retAngle == E_OK) &&
        (angle >= STEER_ANGLE_MIN) &&
        (angle <= STEER_ANGLE_MAX))
    {
        angleFault = FALSE;
    }

    /*
     * 비정상 조향각은 제어에 전달하지 않음
     */
    if (angleFault == TRUE)
    {
        angle = 0;
    }

    /* SafetyManager에 진단 결과 전달 */
    (void)Rte_Write_P_AngleDiagToSafetyManager_AngleFault(
        angleFault);

    /* SafetyManager에 조향각 전달 */
    (void)Rte_Write_P_AngleDiagToSafetyManager_SteerAngle(
        angle);
}
