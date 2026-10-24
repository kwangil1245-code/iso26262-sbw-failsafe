
#include "Rte_SWC_SafetyManager.h"

#define SWC_SafetyManager_START_SEC_CODE
#include "SWC_SafetyManager_MemMap.h"

/*
 * 모든 진단 결과가 정상인 상태를
 * 3회 연속 확인하면 NORMAL 복귀
 */
#define NORMAL_RECOVERY_LIMIT (3U)

/*
 * 안전 상태 관리 변수
 *
 * 초기 상태는 FAIL-SAFE로 설정하여
 * 진단 결과 확인 전 모터 구동 방지
 */
static boolean gIsFailsafe = TRUE;
static uint8 gNormalRecoverCnt = 0U;

void SafetyManager_func(void)
{
    sint16 inSteer = 0;
    sint16 outSteer = 0;

    boolean comFault = TRUE;
    boolean angleFault = TRUE;
    boolean execFault = TRUE;

    boolean curFault = TRUE;
    boolean outFlag = TRUE;

    Std_ReturnType retCom;
    Std_ReturnType retAngle;
    Std_ReturnType retExec;
    Std_ReturnType retSteer;

    /*
     * WdgM 실행 감시 Checkpoint 1
     */
    (void)Rte_Call_SWC_SafetyManager_SE_SafetyManager_CheckpointReached(1U);

    /*
     * 통신 이상 진단 결과 수신
     */
    retCom =
        Rte_Read_R_ComDiagToSafetyManager_ComFault(
            &comFault);

    /*
     * 조향각 유효성 진단 결과 수신
     */
    retAngle =
        Rte_Read_R_AngleDiagToSafetyManager_AngleFault(
            &angleFault);

    /*
     * SW 실행 이상 진단 결과 수신
     */
    retExec =
        Rte_Read_R_ExecDiagToSafetyManager_ExecFault(
            &execFault);

    /*
     * 검증된 조향각 데이터 수신
     */
    retSteer =
        Rte_Read_R_AngleDiagToSafetyManager_SteerAngle(
            &inSteer);

    /*
     * 모든 진단 결과가 정상일 때만
     * 정상 입력으로 판단
     *
     * RTE Read 실패도 Fault로 처리
     */
    if ((retCom == E_OK) &&
        (retAngle == E_OK) &&
        (retExec == E_OK) &&
        (retSteer == E_OK) &&
        (comFault == FALSE) &&
        (angleFault == FALSE) &&
        (execFault == FALSE))
    {
        curFault = FALSE;
    }

    /*
     * Fault 발생 시 즉시 FAIL-SAFE 전환
     */
    if (curFault == TRUE)
    {
        gIsFailsafe = TRUE;

        gNormalRecoverCnt = 0U;
    }
    else
    {
        /*
         * FAIL-SAFE 상태에서
         * 정상 진단 결과 연속 확인
         */
        if (gIsFailsafe == TRUE)
        {
            if (gNormalRecoverCnt <
                NORMAL_RECOVERY_LIMIT)
            {
                gNormalRecoverCnt++;
            }

            /*
             * 정상 상태 3회 연속 확인 시
             * NORMAL 복귀
             */
            if (gNormalRecoverCnt >=
                NORMAL_RECOVERY_LIMIT)
            {
                gIsFailsafe = FALSE;

                gNormalRecoverCnt = 0U;
            }
        }
    }

    /*
     * 최종 안전 상태에 따른 출력 결정
     */
    if (gIsFailsafe == TRUE)
    {
        outFlag = TRUE;
        outSteer = 0;
    }
    else
    {
        outFlag = FALSE;
        outSteer = inSteer;
    }

    /*
     * ControlCalc에 조향각 전달
     */
    (void)Rte_Write_P_SafetyManagerToControlCalc_Steerinfo(
        outSteer);

    /*
     * ControlCalc에 Fail-Safe 상태 전달
     */
    (void)Rte_Write_P_SafetyManagerToControlCalc_flag(
        outFlag);

    /*
     * WdgM 실행 감시 Checkpoint 2
     */
    (void)Rte_Call_SWC_SafetyManager_SE_SafetyManager_CheckpointReached(2U);
}

#define SWC_SafetyManager_STOP_SEC_CODE
#include "SWC_SafetyManager_MemMap.h"
