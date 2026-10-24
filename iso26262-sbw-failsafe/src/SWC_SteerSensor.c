
#include "Rte_SWC_SteeringSensor.h"

static uint8 aliveCounter = 0U;

void RE_Can_Tx_10ms(void)
{
    IoHwAb_ValueType level;
    sint16 angle;

    (void)Rte_Call_R_Potentiometer_ReadDirect(&level, 1U);
    angle = (sint16)(level - 512);

    (void)Rte_Write_P_SteeringSensorData_AliveCounter(aliveCounter++);
    (void)Rte_Write_P_SteeringSensorData_SteeringAngle(angle);
}
