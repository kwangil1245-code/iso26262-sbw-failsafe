
#include "Rte_SWC_ComDiag.h"

#define SEQUENCE_ERROR_LIMIT (2U)
#define RECOVERY_VALID_LIMIT (3U)

typedef enum
{
    ALIVE_STATE_INIT = 0,
    ALIVE_STATE_NORMAL,
    ALIVE_STATE_RECOVERY_FIRST,
    ALIVE_STATE_RECOVERY_CHECK

} AliveDiagState;

static AliveDiagState aliveState = ALIVE_STATE_INIT;

static uint8 prevAliveCounter = 0U;
static uint8 sequenceErrorCnt = 0U;
static uint8 recoveryCount = 0U;

void ComDiag_func(void)
{
    uint8 aliveCounter = 0U;
    uint8 expectedAliveCounter = 0U;

    boolean communicationFault = TRUE;
    Std_ReturnType retAlive;

    retAlive = Rte_Read_R_SteeringSensorData_AliveCounter(
        &aliveCounter);

    if (retAlive != E_OK)
    {
        aliveState = ALIVE_STATE_RECOVERY_FIRST;
        sequenceErrorCnt = 0U;
        recoveryCount = 0U;
    }
    else
    {
        switch (aliveState)
        {
            case ALIVE_STATE_INIT:
            {
                prevAliveCounter = aliveCounter;
                sequenceErrorCnt = 0U;
                recoveryCount = 0U;
                aliveState = ALIVE_STATE_RECOVERY_CHECK;
                break;
            }

            case ALIVE_STATE_NORMAL:
            {
                expectedAliveCounter =
                    (uint8)(prevAliveCounter + 1U);

                if (aliveCounter == expectedAliveCounter)
                {
                    sequenceErrorCnt = 0U;
                    prevAliveCounter = aliveCounter;
                }
                else
                {
                    if (sequenceErrorCnt < SEQUENCE_ERROR_LIMIT)
                    {
                        sequenceErrorCnt++;
                    }

                    if (sequenceErrorCnt >= SEQUENCE_ERROR_LIMIT)
                    {
                        aliveState = ALIVE_STATE_RECOVERY_FIRST;
                        sequenceErrorCnt = 0U;
                        recoveryCount = 0U;
                    }
                    else
                    {
                        prevAliveCounter = aliveCounter;
                    }
                }
                break;
            }

            case ALIVE_STATE_RECOVERY_FIRST:
            {
                prevAliveCounter = aliveCounter;
                recoveryCount = 0U;
                aliveState = ALIVE_STATE_RECOVERY_CHECK;
                break;
            }

            case ALIVE_STATE_RECOVERY_CHECK:
            {
                expectedAliveCounter =
                    (uint8)(prevAliveCounter + 1U);

                if (aliveCounter == expectedAliveCounter)
                {
                    recoveryCount++;
                    prevAliveCounter = aliveCounter;

                    if (recoveryCount >= RECOVERY_VALID_LIMIT)
                    {
                        aliveState = ALIVE_STATE_NORMAL;
                        sequenceErrorCnt = 0U;
                        recoveryCount = 0U;
                    }
                }
                else
                {
                    aliveState = ALIVE_STATE_RECOVERY_FIRST;
                    recoveryCount = 0U;
                }
                break;
            }

            default:
            {
                aliveState = ALIVE_STATE_RECOVERY_FIRST;
                sequenceErrorCnt = 0U;
                recoveryCount = 0U;
                break;
            }
        }
    }

    if ((retAlive == E_OK) &&
        (aliveState == ALIVE_STATE_NORMAL))
    {
        communicationFault = FALSE;
    }

    (void)Rte_Write_P_CommunicationDiagnostic_CommunicationFault(
        communicationFault);
}
