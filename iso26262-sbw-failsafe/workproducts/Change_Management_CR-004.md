# 변경관리 시나리오: Alive Counter 연속 증가 확인 후 FAIL-SAFE 복귀

**Change Request ID**: CR-004  
**Title**: Alive Counter 정상 복귀 판정 강화  
**Version**: 0.1  
**Date**: 2026-08-27  
**Status**: Planned  
**Project**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**관련 결함**: DEF-INT-003  
**관련 SWC**: CanMonitor, SafetyPolicy  

---

## 1. 변경 배경 및 목적

초기 설계에서는 동일한 Alive Counter가 연속 수신되면 통신 갱신 이상으로 판단하여 FAIL-SAFE 상태로 전환하였다.

그러나 기존 로직은 FAIL-SAFE 진입 당시 Alive Counter보다 1 증가한 정상 Counter가 한 번 수신되면 통신이 복구된 것으로 판단하여 즉시 NORMAL 상태로 복귀하였다.

예를 들어 Alive Counter가 `2 → 2`로 수신되면 갱신 이상으로 FAIL-SAFE 상태에 진입하지만, 다음 주기에 예상값인 `3`이 한 번 수신되면 즉시 NORMAL 상태로 복귀하였다.

```text
2 → 2 → FAIL-SAFE
        ↓
        3 → NORMAL 즉시 복귀
```

이 방식은 Alive Counter가 정상 순서로 한 번 증가한 것만 확인하고 통신이 안정적으로 복구되었다고 판단한다.

따라서 다음과 같이 통신 갱신 이상이 다시 발생하면 FAIL-SAFE와 NORMAL 상태가 반복적으로 전환될 수 있다.

```text
2 → 2 → 3 → 3 → 4
    ↓    ↓    ↓    ↓
   FAIL NORMAL FAIL NORMAL
```

본 변경의 목적은 FAIL-SAFE 진입에 사용된 Alive Counter를 복구 판단에서 제외하고, FAIL-SAFE 진입 이후 수신된 Counter들의 연속적인 증가를 확인한 후에만 NORMAL 상태로 복귀하도록 하는 것이다.

## 2. 변경요청

| 항목 | 내용 |
|---|---|
| 발견 단계 | SW 통합검증 |
| 관련 결함 | DEF-INT-003 |
| 관련 시험 | ITC-SW-018 |
| 변경 전 | FAIL-SAFE 진입 당시 Counter보다 1 증가한 정상 Counter가 1회 수신되면 즉시 복귀 |
| 문제점 | 한 번의 정상 Counter 수신만으로 통신 복구를 확정함 |
| 변경 후 | FAIL-SAFE 진입 이후 최초 수신값을 새로운 복구 기준값으로 설정하고, 이후 1씩 증가하는 상태가 2회 연속 확인되면 복귀 |
| 복구 기준 | FAIL-SAFE 진입 이후 최초로 수신된 Alive Counter |
| FAIL-SAFE 진입값 | 정상 복귀 판단에서 제외 |
| 불연속 수신 시 | 연속 증가 횟수를 0으로 초기화하고 현재 Counter를 새로운 기준값으로 설정 |
| Counter 순환 | Counter 최댓값 이후 0으로 변경되는 경우 정상 증가로 인정 |
| 변경 유형 | FAIL-SAFE 정상 복귀 판정 로직 변경 |

## 3. 변경 전 문제 재현

### 3.1 재현 절차

1. Alive Counter `2`를 정상 수신한다.
2. 다음 주기에 동일한 Alive Counter `2`를 수신한다.
3. CanMonitor가 Alive Counter 갱신 이상을 감지한다.
4. SafetyPolicy가 FAIL-SAFE 상태로 전환한다.
5. 다음 주기에 Alive Counter `3`을 수신한다.
6. 기존 CanMonitor가 `3`을 정상적인 다음 Counter로 판단하여 이상 상태를 해제한다.
7. SafetyPolicy가 즉시 NORMAL 상태로 복귀한다.

### 3.2 변경 전 동작

| 수신 순서 | Alive Counter | 판단 | 시스템 상태 |
|---:|---:|---|---|
| 1 | 2 | 정상 수신 | NORMAL |
| 2 | 2 | 동일 Counter 수신 | FAIL-SAFE 진입 |
| 3 | 3 | FAIL-SAFE 진입값보다 1 증가 | NORMAL 즉시 복귀 |

Alive Counter `3`이 정상적인 다음 값인 것은 맞지만, 그 이후에도 `4`, `5`와 같이 정상 증가가 유지되는지는 확인되지 않았다.

따라서 Counter가 한 번 정상적으로 증가한 것만으로 통신 복구를 확정하는 것은 충분하지 않다.

### 3.3 상태 반복 전환 문제

| 수신 순서 | Alive Counter | 기존 판단 | 시스템 상태 |
|---:|---:|---|---|
| 1 | 2 | 정상 수신 | NORMAL |
| 2 | 2 | 동일 Counter | FAIL-SAFE |
| 3 | 3 | 1 증가 | NORMAL |
| 4 | 3 | 동일 Counter | FAIL-SAFE |
| 5 | 4 | 1 증가 | NORMAL |

기존 로직에서는 시스템 상태가 다음과 같이 반복될 수 있다.

```text
NORMAL → FAIL-SAFE → NORMAL → FAIL-SAFE → NORMAL
```

이로 인해 통신 상태가 안정적으로 복구되지 않았는데도 모터 출력 차단과 허용이 반복될 수 있다.

## 4. 원인 분석

기존 복구 로직은 FAIL-SAFE 진입 당시 저장된 Alive Counter를 기준으로 다음 Counter가 1 증가했는지만 확인하였다.

```c
if (aliveCounter == GetNextAliveCounter(prevAliveCounter))
{
    aliveFault = FALSE;
}
```

예를 들어 `2 → 2`로 FAIL-SAFE에 진입하면 `prevAliveCounter`에는 `2`가 저장되어 있다.

그다음 `3`이 수신되면 예상값과 일치하므로 즉시 이상 상태가 해제된다.

```text
FAIL-SAFE 진입값: 2
예상 다음 값:     3
실제 수신값:      3
판단 결과:        즉시 정상 복귀
```

문제의 원인은 다음 두 가지이다.

- FAIL-SAFE 진입에 사용된 Counter를 정상 복귀 판단의 기준값으로 사용하였다.
- 정상적인 Counter 증가가 연속적으로 유지되는지 확인하지 않았다.

## 5. 변경 내용

### 5.1 정상 복귀 판정 기준

Alive Counter 갱신 이상으로 FAIL-SAFE 상태에 진입한 경우 다음 절차에 따라 정상 복귀 여부를 판단한다.

1. FAIL-SAFE 진입 당시 사용된 Alive Counter는 정상 복귀 판단에서 제외한다.
2. FAIL-SAFE 진입 이후 최초로 수신된 Alive Counter를 새로운 복구 기준값으로 저장한다.
3. 다음 Alive Counter가 복구 기준값보다 1 증가했는지 확인한다.
4. 정상적으로 1 증가하면 연속 증가 횟수를 1 증가시킨다.
5. Alive Counter가 1씩 증가하는 상태가 2회 연속 확인되면 이상 상태를 해제한다.
6. 연속 증가 확인 중 예상값과 다른 Counter가 수신되면 연속 증가 횟수를 0으로 초기화한다.
7. 불연속으로 수신된 현재 Counter를 새로운 복구 기준값으로 설정하고 다시 연속 증가 여부를 확인한다.

### 5.2 변경 후 정상 복귀 예시

| 수신 순서 | Alive Counter | 복구 판단 | 연속 증가 횟수 | 시스템 상태 |
|---:|---:|---|---:|---|
| 1 | 2 | 정상 수신 | - | NORMAL |
| 2 | 2 | 동일 Counter 수신 | 0 | FAIL-SAFE 진입 |
| 3 | 3 | FAIL-SAFE 이후 최초값 저장 | 0 | FAIL-SAFE 유지 |
| 4 | 4 | 기준값 3보다 1 증가 | 1 | FAIL-SAFE 유지 |
| 5 | 5 | 이전 값 4보다 1 증가 | 2 | NORMAL 복귀 |

변경 후에는 `2 → 2`로 FAIL-SAFE 상태에 진입했을 때, FAIL-SAFE 진입에 사용된 `2`를 복구 기준으로 사용하지 않는다.

FAIL-SAFE 진입 이후 최초로 수신된 `3`을 새로운 기준값으로 저장하고, 이후 `4`, `5`가 순서대로 수신되어 연속 증가가 2회 확인된 시점에 NORMAL 상태로 복귀한다.

```text
2 → 2 → FAIL-SAFE
        ↓
        3 → 복구 기준값 저장
        ↓
        4 → 연속 증가 1회
        ↓
        5 → 연속 증가 2회
        ↓
       NORMAL 복귀
```

### 5.3 복구 중 동일 Counter 수신

| 수신 순서 | Alive Counter | 복구 판단 | 연속 증가 횟수 | 시스템 상태 |
|---:|---:|---|---:|---|
| 1 | 2 | 정상 수신 | - | NORMAL |
| 2 | 2 | 동일 Counter 수신 | 0 | FAIL-SAFE 진입 |
| 3 | 3 | 복구 기준값 저장 | 0 | FAIL-SAFE 유지 |
| 4 | 4 | 정상 증가 | 1 | FAIL-SAFE 유지 |
| 5 | 4 | 동일 Counter 수신 | 0 | FAIL-SAFE 유지 |
| 6 | 5 | 기준값 4보다 1 증가 | 1 | FAIL-SAFE 유지 |
| 7 | 6 | 이전 값 5보다 1 증가 | 2 | NORMAL 복귀 |

복구 확인 중 동일한 Counter가 수신되면 연속 증가가 끊긴 것으로 판단한다.

현재 Counter를 새로운 기준값으로 설정한 후 다시 2회의 연속 증가를 확인한다.

### 5.4 복구 중 불연속 Counter 수신

| 수신 순서 | Alive Counter | 복구 판단 | 연속 증가 횟수 | 시스템 상태 |
|---:|---:|---|---:|---|
| 1 | 2 | 정상 수신 | - | NORMAL |
| 2 | 2 | 동일 Counter 수신 | 0 | FAIL-SAFE 진입 |
| 3 | 3 | 복구 기준값 저장 | 0 | FAIL-SAFE 유지 |
| 4 | 4 | 정상 증가 | 1 | FAIL-SAFE 유지 |
| 5 | 7 | 예상값 5와 불일치 | 0 | FAIL-SAFE 유지 |
| 6 | 8 | 기준값 7보다 1 증가 | 1 | FAIL-SAFE 유지 |
| 7 | 9 | 이전 값 8보다 1 증가 | 2 | NORMAL 복귀 |

Counter `7`이 수신된 시점에는 연속 증가 횟수를 초기화하고, `7`을 새로운 복구 기준값으로 설정한다.

### 5.5 Alive Counter 순환 처리

Alive Counter 범위가 `0~15`인 경우 `15 → 0`은 정상적인 증가로 인정한다.

| 이전 Counter | 현재 Counter | 판단 |
|---:|---:|---|
| 14 | 15 | 정상 증가 |
| 15 | 0 | 정상 순환 |
| 0 | 1 | 정상 증가 |
| 15 | 1 | 불연속 Counter |

예를 들어 FAIL-SAFE 진입 이후 다음과 같이 수신되면 정상 복귀가 가능하다.

```text
14 → 복구 기준값
15 → 연속 증가 1회
 0 → 연속 증가 2회
   → NORMAL 복귀
```

## 6. 요구사항 변경

### 6.1 시스템 요구사항 Req_008 변경

**변경 전**

> Alive Counter 갱신 이상이 해제되면 시스템은 FAIL-SAFE 상태에서 NORMAL 상태로 복귀해야 한다.

**변경 후**

> Alive Counter 갱신 이상으로 FAIL-SAFE 상태에 진입한 경우, 시스템은 FAIL-SAFE 진입 이후 수신된 Alive Counter가 정상적인 순서로 연속 증가하는 것을 확인한 후에만 NORMAL 상태로 복귀해야 한다. FAIL-SAFE 진입에 사용된 Alive Counter는 정상 복귀 판단에서 제외해야 한다.

### 6.2 SW 요구사항 변경

> **SWR-MON-006**: CanMonitor는 동일한 Alive Counter가 연속 수신되면 Alive Counter 갱신 이상 상태를 활성화해야 한다.

> **SWR-MON-007**: CanMonitor는 FAIL-SAFE 진입 당시 사용된 Alive Counter를 정상 복귀 판단의 기준값으로 사용하지 않아야 한다.

> **SWR-MON-008**: CanMonitor는 FAIL-SAFE 진입 이후 최초로 수신된 Alive Counter를 정상 복귀 확인의 기준값으로 저장해야 한다.

> **SWR-MON-009**: CanMonitor는 현재 Alive Counter가 이전 Alive Counter보다 1 증가한 경우 정상 증가로 판단해야 한다.

> **SWR-MON-010**: CanMonitor는 FAIL-SAFE 진입 이후 Alive Counter의 정상 증가가 2회 연속 확인된 경우에만 Alive Counter 갱신 이상 상태를 해제해야 한다.

> **SWR-MON-011**: 정상 증가 확인 중 예상값과 다른 Alive Counter가 수신되면 CanMonitor는 연속 증가 횟수를 0으로 초기화하고 현재 Counter를 새로운 복구 기준값으로 저장해야 한다.

> **SWR-MON-012**: Alive Counter가 최댓값에 도달한 후 0으로 변경되는 경우 CanMonitor는 이를 정상 증가로 판단해야 한다.

## 7. 추적성 및 영향 분석

```text
DEF-INT-003
└─ CR-004
   └─ Req_008
      └─ SYS-F-008 / SYS-DES-008
         └─ SWR-MON-006–012
            └─ SWC-002 CanMonitor
               └─ App_CanMonitor.c
                  ├─ UT-MON-010–016
                  ├─ ITC-SW-018–020
                  └─ SYS-TC-REC-007–009
```

| 대상 | 영향 구분 | 조치 |
|---|---|---|
| Req_008 | 직접 영향 | Alive Counter 정상 복귀 조건 구체화 |
| System Design | 직접 영향 | FAIL-SAFE 복귀 조건에 연속 증가 확인 추가 |
| SWR-MON-006–012 | 신규·변경 | 복구 기준값, 연속 증가 및 순환 조건 정의 |
| App_CanMonitor.c | 직접 영향 | 복구 기준값과 연속 증가 Counter 추가 |
| App_SafetyPolicy.c | 간접 영향 | CanMonitor Flag가 해제될 때까지 FAIL-SAFE 유지 |
| App_ControlCalc.c | 회귀 영향 | FAIL-SAFE 유지 중 PWM 출력 차단 확인 |
| App_Pwm_Actuator.c | 회귀 영향 | FAIL-SAFE 유지 중 모터 구동 차단 확인 |
| AUTOSAR RTE Interface | 영향 없음 | 기존 조향값 및 Boolean Flag Interface 유지 |
| 단위시험 | 직접 영향 | 최초값, 연속 증가, 불연속 및 순환 시험 추가 |
| 통합시험 | 직접 영향 | CanMonitor에서 SafetyPolicy까지 상태 전달 확인 |
| 시스템시험 | 직접 영향 | CANoe 기반 FAIL-SAFE 복귀 시나리오 변경 |

## 8. 설계 및 구현 변경

### 8.1 내부 상태 변수

| 변수 | 자료형 | 초기값 | 용도 |
|---|---|---:|---|
| `prevAliveCounter` | `uint8` | 0 | 이전 Alive Counter 저장 |
| `firstValid` | `boolean` | `FALSE` | 최초 유효 Counter 수신 여부 |
| `aliveFaultLatched` | `boolean` | `FALSE` | Alive Counter 갱신 이상 상태 유지 |
| `recoveryBaseValid` | `boolean` | `FALSE` | FAIL-SAFE 이후 복구 기준값 설정 여부 |
| `recoveryCount` | `uint8` | 0 | Alive Counter 연속 증가 확인 횟수 |

### 8.2 설정값

```c
#define ALIVE_COUNTER_MAX          (15U)
#define RECOVERY_SEQUENCE_LIMIT    (2U)
```

### 8.3 다음 Alive Counter 계산

```c
static uint8 GetNextAliveCounter(uint8 counter)
{
    uint8 nextCounter;

    if (counter >= ALIVE_COUNTER_MAX)
    {
        nextCounter = 0U;
    }
    else
    {
        nextCounter = (uint8)(counter + 1U);
    }

    return nextCounter;
}
```

### 8.4 변경 로직 예시

```c
#include "Rte_SWC_CanMonitor.h"

#define STEER_ANGLE_MIN            (-512)
#define STEER_ANGLE_MAX            (511)
#define ALIVE_COUNTER_MAX          (15U)
#define RECOVERY_SEQUENCE_LIMIT    (2U)

static uint8 prevAliveCounter = 0U;
static uint8 recoveryCount = 0U;

static boolean firstValid = FALSE;
static boolean aliveFaultLatched = FALSE;
static boolean recoveryBaseValid = FALSE;

static uint8 GetNextAliveCounter(uint8 counter)
{
    uint8 nextCounter;

    if (counter >= ALIVE_COUNTER_MAX)
    {
        nextCounter = 0U;
    }
    else
    {
        nextCounter = (uint8)(counter + 1U);
    }

    return nextCounter;
}

void CanMonitor_func(void)
{
    uint8 aliveCounter = 0U;
    sint16 angle = 0;

    boolean flag = FALSE;
    boolean angleFault = FALSE;
    boolean rteFault = FALSE;

    Std_ReturnType retAlive;
    Std_ReturnType retAngle;

    retAlive =
        Rte_Read_Project_SSU_SteerInfo_SSU_AliveCounter(
            &aliveCounter);

    retAngle =
        Rte_Read_Project_SSU_SteerInfo_SSU_SteerAngle(
            &angle);

    if ((retAlive != RTE_E_OK) ||
        (retAngle != RTE_E_OK))
    {
        rteFault = TRUE;

        recoveryBaseValid = FALSE;
        recoveryCount = 0U;
    }
    else
    {
        if (firstValid == FALSE)
        {
            prevAliveCounter = aliveCounter;
            firstValid = TRUE;
        }
        else if (aliveFaultLatched == FALSE)
        {
            if (aliveCounter == prevAliveCounter)
            {
                /*
                 * 동일 Counter 수신:
                 * Alive Counter 갱신 이상 활성화
                 */
                aliveFaultLatched = TRUE;

                /*
                 * FAIL-SAFE 진입에 사용된 Counter는
                 * 복구 판단에서 제외한다.
                 */
                recoveryBaseValid = FALSE;
                recoveryCount = 0U;
            }

            prevAliveCounter = aliveCounter;
        }
        else
        {
            if (recoveryBaseValid == FALSE)
            {
                /*
                 * FAIL-SAFE 진입 이후 최초 수신값을
                 * 새로운 복구 기준값으로 저장한다.
                 */
                prevAliveCounter = aliveCounter;
                recoveryBaseValid = TRUE;
                recoveryCount = 0U;
            }
            else if (aliveCounter ==
                     GetNextAliveCounter(prevAliveCounter))
            {
                /*
                 * 이전 Counter보다 1 증가:
                 * 연속 증가 횟수 증가
                 */
                prevAliveCounter = aliveCounter;
                recoveryCount++;

                if (recoveryCount >=
                    RECOVERY_SEQUENCE_LIMIT)
                {
                    aliveFaultLatched = FALSE;
                    recoveryBaseValid = FALSE;
                    recoveryCount = 0U;
                }
            }
            else
            {
                /*
                 * 연속 증가 실패:
                 * 현재 값을 새로운 복구 기준값으로 설정
                 */
                prevAliveCounter = aliveCounter;
                recoveryCount = 0U;
            }
        }

        if ((angle < STEER_ANGLE_MIN) ||
            (angle > STEER_ANGLE_MAX))
        {
            angleFault = TRUE;
        }
    }

    if ((aliveFaultLatched == TRUE) ||
        (angleFault == TRUE) ||
        (rteFault == TRUE))
    {
        flag = TRUE;
    }

    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Steer_info(
        angle);

    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Flag(
        flag);
}
```

## 9. 처리 흐름

```text
정상 상태
   │
   ├─ Alive Counter 정상 증가
   │      └─ NORMAL 유지
   │
   └─ 동일 Counter 수신
          ↓
      Alive Counter 이상 활성화
          ↓
      FAIL-SAFE 진입
          ↓
      FAIL-SAFE 이후 최초 Counter 수신
          ↓
      해당 Counter를 복구 기준값으로 저장
          ↓
      다음 Counter가 1 증가했는가?
          ├─ 아니요: 연속 증가 횟수 초기화
          │          현재 Counter를 새 기준값으로 저장
          │
          └─ 예: 연속 증가 횟수 +1
                    ↓
              연속 증가 2회인가?
                  ├─ 아니요: FAIL-SAFE 유지
                  └─ 예: Alive Counter 이상 해제
                              ↓
                          NORMAL 복귀
```

## 10. SWC별 변경 책임

### 10.1 CanMonitor

- Alive Counter 갱신 이상 감지
- Alive Counter 이상 상태 유지
- FAIL-SAFE 진입에 사용된 Counter를 복구 판단에서 제외
- FAIL-SAFE 진입 이후 최초 Counter를 복구 기준값으로 저장
- Alive Counter의 연속 증가 2회 확인
- 불연속 Counter 수신 시 연속 증가 횟수 초기화
- Counter 최댓값 이후 0으로 순환하는 조건 처리
- 복구 조건 충족 전까지 SafetyPolicy에 Flag 유지

### 10.2 SafetyPolicy

- CanMonitor에서 전달된 Flag가 `TRUE`이면 FAIL-SAFE 유지
- CanMonitor가 복구 조건을 충족하여 Flag를 해제한 후 NORMAL 복귀 허용
- Alive Counter 증가 여부를 직접 판단하지 않음

### 10.3 ControlCalc 및 PwmActuator

- FAIL-SAFE 상태가 유지되는 동안 PWM Duty와 방향 출력 차단
- SafetyPolicy가 NORMAL 복귀를 허용한 후 제어 출력 재개
- CR-003의 정상 복귀 기준값 동기화 로직과 함께 회귀시험 수행

## 11. 검증 변경 및 회귀시험

### 11.1 단위시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| UT-MON-010 | Counter `2 → 2` 수신 | Alive Counter 이상 상태 활성화 |
| UT-MON-011 | FAIL-SAFE 후 Counter `3` 수신 | `3`을 복구 기준값으로 저장하고 FAIL-SAFE 유지 |
| UT-MON-012 | FAIL-SAFE 후 `3 → 4` 수신 | 연속 증가 1회, FAIL-SAFE 유지 |
| UT-MON-013 | FAIL-SAFE 후 `3 → 4 → 5` 수신 | 연속 증가 2회 후 이상 상태 해제 |
| UT-MON-014 | 복구 중 `3 → 4 → 4` 수신 | 연속 증가 횟수 0으로 초기화 |
| UT-MON-015 | 복구 중 `3 → 4 → 7 → 8 → 9` 수신 | `7`을 새 기준값으로 설정하고 `9`에서 이상 상태 해제 |
| UT-MON-016 | `14 → 15 → 0` 수신 | Counter 순환을 포함한 연속 증가 2회 인정 |

### 11.2 통합시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| ITC-SW-018 | Counter `2 → 2` 주입 | CanMonitor Flag 활성화 및 SafetyPolicy FAIL-SAFE 전환 |
| ITC-SW-019 | FAIL-SAFE 후 `3 → 4` 주입 | CanMonitor Flag 유지 및 SafetyPolicy FAIL-SAFE 유지 |
| ITC-SW-020 | FAIL-SAFE 후 `3 → 4 → 5` 주입 | CanMonitor Flag 해제 및 SafetyPolicy NORMAL 복귀 허용 |

### 11.3 시스템시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| SYS-TC-REC-007 | CANoe에서 `2 → 2 → 3` 송신 | `3`을 복구 기준값으로 저장하고 FAIL-SAFE 유지 |
| SYS-TC-REC-008 | CANoe에서 `2 → 2 → 3 → 4` 송신 | 연속 증가 1회이므로 FAIL-SAFE 유지 |
| SYS-TC-REC-009 | CANoe에서 `2 → 2 → 3 → 4 → 5` 송신 | 연속 증가 2회 후 NORMAL 복귀 |
| SYS-TC-REC-010 | CANoe에서 `2 → 2 → 3 → 4 → 7 → 8 → 9` 송신 | 불연속 시 복구 초기화 후 `9`에서 NORMAL 복귀 |

### 11.4 회귀시험

회귀시험은 다음 항목을 대상으로 수행한다.

- 정상 Alive Counter 수신 시 오검출 여부
- 동일 Alive Counter 수신 시 FAIL-SAFE 진입
- FAIL-SAFE 진입값이 복구 기준에서 제외되는지 확인
- 복구 최초값 수신 후 FAIL-SAFE 유지 여부
- 연속 증가 1회 확인 후 FAIL-SAFE 유지 여부
- 연속 증가 2회 확인 후 NORMAL 복귀 여부
- 불연속 Counter 수신 시 복구 확인 초기화
- Alive Counter 최댓값 이후 0으로의 정상 순환 처리
- FAIL-SAFE 상태에서 PWM Duty 0 유지
- FAIL-SAFE 상태에서 Left/Right 방향 출력 차단
- 정상 복귀 후 CR-003의 제어 기준값 동기화
- 기준값 동기화 다음 주기부터 정상 제어 재개

## 12. CR-003과의 관계

CR-003과 CR-004는 FAIL-SAFE 정상 복귀 과정에서 발생하는 서로 다른 문제를 해결한다.

| 변경요청 | 문제 | 해결 방법 |
|---|---|---|
| CR-003 | FAIL-SAFE 중 `prev_input_steer`가 0으로 변경되어 복귀 순간 급격한 모터 출력 발생 | 정상 복귀 첫 주기에 현재 조향 입력으로 제어 기준값 동기화 |
| CR-004 | FAIL-SAFE 진입값보다 1 증가한 Counter가 한 번 수신되면 즉시 NORMAL 복귀 | FAIL-SAFE 이후 최초 Counter를 새 기준값으로 설정하고 연속 증가 2회 확인 후 복귀 |

두 변경이 모두 적용된 경우 정상 복귀 순서는 다음과 같다.

```text
Alive Counter 갱신 이상 감지
        ↓
FAIL-SAFE 진입 및 모터 출력 차단
        ↓
FAIL-SAFE 진입에 사용된 Counter 제외
        ↓
FAIL-SAFE 이후 최초 Counter를 복구 기준값으로 저장
        ↓
Alive Counter 연속 증가 1회 확인
        ↓
Alive Counter 연속 증가 2회 확인
        ↓
CanMonitor 이상 상태 해제
        ↓
SafetyPolicy NORMAL 복귀 허용
        ↓
ControlCalc 현재 조향값 기준 동기화
        ↓
다음 주기부터 정상 제어 재개
```

## 13. 형상 및 버전관리 계획

### 13.1 기준선

```text
BL-02:
FAIL-SAFE 진입값보다 1 증가한 Counter가 한 번 수신되면
즉시 정상 복귀하는 기존 로직

BL-03:
FAIL-SAFE 이후 최초 Counter를 복구 기준값으로 설정하고,
이후 Counter가 1씩 증가하는 상태를 2회 연속 확인한 후
정상 복귀하는 변경 로직
```

### 13.2 Git 관리 예시

```bash
git checkout -b change/CR-004-alive-recovery

git commit -m "CR-004 Update alive counter recovery requirements"
git commit -m "CR-004 Add consecutive alive recovery logic"
git commit -m "CR-004 Add alive recovery unit tests"
git commit -m "CR-004 Add integration and system tests"
git commit -m "CR-004 Update traceability matrix"

git tag -a BL-03 -m "CR-004 verified alive counter recovery baseline"
```

### 13.3 변경 대상 산출물

| 산출물 | 변경 내용 |
|---|---|
| `01_Requirements.md` | Req_008에 Alive Counter 연속 증가 복귀 조건 반영 |
| `02_System_Design.md` | FAIL-SAFE 정상 복귀 조건 및 상태 전이 변경 |
| `03_SW_Requirements.md` | SWR-MON-006–012 추가 |
| `04_SW_Architecture_Design.md` | CanMonitor의 복구 판정 책임 반영 |
| `05_SW_Detailed_Design_Unit_Construction.md` | 복구 기준값 및 연속 증가 Counter 처리 추가 |
| `06_SW_Unit_Verification.md` | 최초값, 연속 증가, 불연속 및 순환 시험 추가 |
| `07_SW_Integration_Verification.md` | CanMonitor에서 SafetyPolicy까지 복구 전달 시험 추가 |
| `08_System_Verification.md` | CANoe 기반 정상 복귀 시나리오 변경 |
| `Traceability_Matrix.md` | DEF-INT-003, CR-004, 요구사항, 설계 및 시험 연결 |
| `App_CanMonitor.c` | Alive Counter 복구 기준값 및 연속 증가 2회 확인 로직 추가 |

## 14. 반영 절차

1. 통합검증에서 확인된 즉시 정상 복귀 현상을 DEF-INT-003으로 등록한다.
2. DEF-INT-003과 CR-004를 연결한다.
3. 변경 대상 요구사항, SWC 및 시험 항목에 대한 영향 분석을 수행한다.
4. Req_008과 SWR-MON-006–012를 변경하거나 추가한다.
5. 시스템설계, SW 아키텍처 및 상세설계를 갱신한다.
6. App_CanMonitor.c에 복구 기준값 및 연속 증가 2회 확인 로직을 구현한다.
7. 단위시험으로 최초값, 연속 증가, 불연속 및 Counter 순환 조건을 검증한다.
8. 통합시험으로 CanMonitor와 SafetyPolicy의 상태 전달을 검증한다.
9. CANoe 기반 시스템시험으로 FAIL-SAFE 유지 및 정상 복귀 시점을 확인한다.
10. CR-003을 포함한 정상 복귀 전체 경로의 회귀시험을 수행한다.
11. Traceability Matrix와 관련 문서의 Version 및 변경이력을 갱신한다.
12. 검증 완료 후 CR-004를 Closed로 전환하고 BL-03 Tag를 생성한다.

---

본 문서는 FAIL-SAFE 진입 당시 Counter보다 1 증가한 값이 한 번 수신되면 즉시 정상 복귀하는 문제를 SW 통합검증에서 발견하고, FAIL-SAFE 진입 이후 수신된 Alive Counter의 연속성을 확인하도록 요구사항·설계·구현·시험 및 형상관리 변경을 전개한 포트폴리오용 변경관리 시나리오다.
