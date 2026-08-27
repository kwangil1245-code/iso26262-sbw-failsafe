# 변경관리 시나리오: FAIL-SAFE 진입 Counter의 정상 복구 횟수 제외

**Change Request ID**: CR-004  
**Title**: Alive Counter 정상 복구 확인 시작 시점 수정  
**Version**: 0.1  
**Date**: 2026-08-27  
**Status**: Planned  
**Project**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**관련 결함**: DEF-INT-003  
**관련 SWC**: CanMonitor, SafetyPolicy  

---

## 1. 변경 배경 및 목적

초기 설계에서는 동일한 Alive Counter가 연속 수신되면 Alive Counter 갱신 이상으로 판단하여 FAIL-SAFE 상태로 전환하고, 이후 Alive Counter가 정상적인 순서로 3회 연속 수신되면 NORMAL 상태로 복귀하도록 하였다.

그러나 SW 통합검증에서 FAIL-SAFE 진입에 사용된 마지막 Alive Counter가 정상 복구 확인의 첫 번째 값으로 포함되는 문제를 확인하였다.

예를 들어 Alive Counter `2`가 연속 수신되어 FAIL-SAFE 상태에 진입한 경우, 기존 구현은 FAIL-SAFE 진입에 사용된 `2`와 이후 수신된 `3`, `4`를 정상 Counter 3회로 판단하였다.

```text
2 → 2 → FAIL-SAFE
        │
        └─ 2: 복구 확인 1회로 잘못 포함
             ↓
             3: 복구 확인 2회
             ↓
             4: 복구 확인 3회
             ↓
           NORMAL 복귀
```

그러나 FAIL-SAFE 진입에 사용된 `2`는 Alive Counter 갱신 이상을 발생시킨 값이므로 정상 복구 확인 횟수에 포함해서는 안 된다.

변경 후에는 FAIL-SAFE 진입 시 정상 복구 확인 횟수를 `0`으로 초기화하고, FAIL-SAFE 진입 이후 수신된 정상 Counter만 복구 확인 횟수에 포함한다.

```text
2 → 2 → FAIL-SAFE
        │
        └─ FAIL-SAFE 진입값 2는 복구 확인에서 제외
             ↓
             3: 정상 복구 확인 1회
             ↓
             4: 정상 복구 확인 2회
             ↓
             5: 정상 복구 확인 3회
             ↓
           NORMAL 복귀
```

본 변경의 목적은 기존의 정상 Counter 3회 확인 요구사항은 유지하면서, 정상 복구 확인의 시작 시점을 FAIL-SAFE 진입 이후로 수정하는 것이다.

## 2. 변경요청

| 항목 | 내용 |
|---|---|
| 발견 단계 | SW 통합검증 |
| 관련 결함 | DEF-INT-003 |
| 관련 시험 | ITC-SW-018 |
| 기존 복구 조건 | 정상 순서의 Alive Counter 3회 연속 확인 |
| 변경 전 구현 | FAIL-SAFE 진입값을 복구 확인 1회로 포함 |
| 변경 전 복귀 예시 | `2 → 2`로 FAIL-SAFE 진입 후 `3 → 4` 수신 시 복귀 |
| 문제점 | 비정상 상태를 발생시킨 Counter가 정상 복구값으로 포함됨 |
| 변경 후 구현 | FAIL-SAFE 진입 시 복구 횟수를 0으로 초기화하고 이후 정상 Counter만 확인 |
| 변경 후 복귀 예시 | `2 → 2`로 FAIL-SAFE 진입 후 `3 → 4 → 5` 수신 시 복귀 |
| 불연속 수신 시 | 복구 확인 횟수를 0으로 초기화하고 FAIL-SAFE 유지 |
| 변경 유형 | 정상 복구 Counter 초기화 및 산정 기준 수정 |
| 요구사항 변경 여부 | 정상 Counter 3회 확인 조건은 유지하고 상세 조건을 명확화 |

## 3. 변경 전 문제 재현

### 3.1 재현 절차

1. Alive Counter `2`를 정상 수신한다.
2. 다음 주기에 동일한 Alive Counter `2`를 다시 수신한다.
3. CanMonitor가 Alive Counter 갱신 이상을 감지한다.
4. SafetyPolicy가 FAIL-SAFE 상태로 전환한다.
5. 기존 구현이 FAIL-SAFE 진입에 사용된 `2`를 정상 복구 확인 1회로 포함한다.
6. 다음 주기에 Counter `3`이 수신되면 복구 확인 2회로 판단한다.
7. 다음 주기에 Counter `4`가 수신되면 복구 확인 3회로 판단한다.
8. CanMonitor가 이상 상태를 해제하고 SafetyPolicy가 NORMAL 상태로 복귀한다.

### 3.2 변경 전 동작

| 수신 순서 | Alive Counter | 판단 | 복구 확인 횟수 | 시스템 상태 |
|---:|---:|---|---:|---|
| 1 | 2 | 정상 수신 | - | NORMAL |
| 2 | 2 | 동일 Counter 수신 | 1로 잘못 설정 | FAIL-SAFE 진입 |
| 3 | 3 | 정상 순서 수신 | 2 | FAIL-SAFE 유지 |
| 4 | 4 | 정상 순서 수신 | 3 | NORMAL 복귀 |

기존 구현에서는 FAIL-SAFE 진입 이후 실제로 정상 수신된 Counter가 `3`, `4`의 2개뿐인데도 정상 Counter가 3회 확인된 것으로 판단하였다.

### 3.3 요구사항과 구현의 불일치

기존 요구사항의 의도는 FAIL-SAFE 진입 이후 정상적인 Alive Counter를 3회 확인하는 것이었다.

```text
요구사항 의도

FAIL-SAFE 진입
      ↓
정상 Counter 1회
      ↓
정상 Counter 2회
      ↓
정상 Counter 3회
      ↓
NORMAL 복귀
```

하지만 기존 구현은 다음과 같이 동작하였다.

```text
기존 구현

FAIL-SAFE 진입값
      ↓
복구 확인 1회로 포함
      ↓
정상 Counter 2회
      ↓
정상 Counter 3회
      ↓
NORMAL 복귀
```

따라서 요구사항에는 정상 Counter 3회 확인이 명시되어 있었지만, 실제 구현에서는 FAIL-SAFE 이후 정상 Counter를 2회만 확인하고 복귀하였다.

## 4. 원인 분석

문제의 직접적인 원인은 FAIL-SAFE 진입 시 복구 확인 횟수를 `1`로 설정하거나, FAIL-SAFE 진입에 사용된 현재 Counter를 정상 복구값으로 포함한 것이다.

### 4.1 변경 전 로직 예시

```c
if (aliveCounter == prevAliveCounter)
{
    aliveFaultLatched = TRUE;

    /*
     * FAIL-SAFE 진입값을 복구 확인 1회로
     * 잘못 포함한 기존 로직
     */
    recoveryCount = 1U;
}
```

동일한 Counter가 수신된 것은 Alive Counter가 정상적으로 증가하지 않았다는 의미이다.

따라서 해당 Counter는 정상 복구 확인값이 아니라 FAIL-SAFE 진입 원인에 해당한다.

### 4.2 원인 정리

- FAIL-SAFE 진입 조건과 정상 복구 조건의 Counter 산정 기준이 분리되지 않았다.
- FAIL-SAFE 진입에 사용된 비정상 Counter가 복구 확인값으로 재사용되었다.
- FAIL-SAFE 진입 시 `recoveryCount`가 0으로 초기화되지 않았다.
- 요구사항의 “정상 Counter 3회”에 대한 시작 시점이 상세설계에 명확하게 정의되지 않았다.

## 5. 변경 내용

### 5.1 변경 원칙

1. FAIL-SAFE 진입에 사용된 Alive Counter는 정상 복구 확인 횟수에 포함하지 않는다.
2. FAIL-SAFE 진입 시 `recoveryCount`를 `0`으로 초기화한다.
3. FAIL-SAFE 진입 이후 수신된 Alive Counter부터 정상 복구 확인을 시작한다.
4. 현재 Counter가 이전 Counter보다 1 증가한 경우 정상 복구 확인 횟수를 증가시킨다.
5. 정상적인 Counter 증가가 3회 연속 확인되면 Alive Counter 이상 상태를 해제한다.
6. 정상 복구 확인 중 동일하거나 불연속적인 Counter가 수신되면 복구 확인 횟수를 0으로 초기화한다.
7. Alive Counter 최댓값 이후 0으로 변경되는 경우 정상적인 순환으로 인정한다.

### 5.2 변경 전후 비교

| 구분 | 복구 확인에 포함되는 Counter | NORMAL 복귀 시점 |
|---|---|---|
| 변경 전 | FAIL-SAFE 진입값 `2` + 정상값 `3`, `4` | `4` 수신 시 |
| 변경 후 | FAIL-SAFE 이후 정상값 `3`, `4`, `5` | `5` 수신 시 |

### 5.3 변경 후 정상 복구 동작

| 수신 순서 | Alive Counter | 판단 | 복구 확인 횟수 | 시스템 상태 |
|---:|---:|---|---:|---|
| 1 | 2 | 정상 수신 | - | NORMAL |
| 2 | 2 | 동일 Counter 수신 | 0으로 초기화 | FAIL-SAFE 진입 |
| 3 | 3 | FAIL-SAFE 이후 정상 수신 1회 | 1 | FAIL-SAFE 유지 |
| 4 | 4 | 정상 수신 2회 | 2 | FAIL-SAFE 유지 |
| 5 | 5 | 정상 수신 3회 | 3 | NORMAL 복귀 |

```text
변경 전
2 → 2(FS, 복구 1회로 잘못 포함) → 3(2회) → 4(3회, 복귀)

변경 후
2 → 2(FS, 복구 횟수 0) → 3(1회) → 4(2회) → 5(3회, 복귀)
```

### 5.4 복구 중 동일 Counter 수신

| 수신 순서 | Alive Counter | 판단 | 복구 확인 횟수 | 시스템 상태 |
|---:|---:|---|---:|---|
| 1 | 2 | 정상 수신 | - | NORMAL |
| 2 | 2 | 동일 Counter 수신 | 0 | FAIL-SAFE 진입 |
| 3 | 3 | 정상 복구 확인 | 1 | FAIL-SAFE 유지 |
| 4 | 4 | 정상 복구 확인 | 2 | FAIL-SAFE 유지 |
| 5 | 4 | 동일 Counter 수신 | 0 | FAIL-SAFE 유지 |
| 6 | 5 | 정상 복구 확인 | 1 | FAIL-SAFE 유지 |
| 7 | 6 | 정상 복구 확인 | 2 | FAIL-SAFE 유지 |
| 8 | 7 | 정상 복구 확인 | 3 | NORMAL 복귀 |

복구 중 동일한 Counter가 다시 수신되면 통신이 안정적으로 복구되지 않은 것으로 판단하고 복구 확인을 처음부터 다시 수행한다.

### 5.5 복구 중 불연속 Counter 수신

| 수신 순서 | Alive Counter | 판단 | 복구 확인 횟수 | 시스템 상태 |
|---:|---:|---|---:|---|
| 1 | 2 | 정상 수신 | - | NORMAL |
| 2 | 2 | 동일 Counter 수신 | 0 | FAIL-SAFE 진입 |
| 3 | 3 | 정상 복구 확인 | 1 | FAIL-SAFE 유지 |
| 4 | 4 | 정상 복구 확인 | 2 | FAIL-SAFE 유지 |
| 5 | 8 | 예상값 5와 불일치 | 0 | FAIL-SAFE 유지 |
| 6 | 9 | 정상 복구 확인 | 1 | FAIL-SAFE 유지 |
| 7 | 10 | 정상 복구 확인 | 2 | FAIL-SAFE 유지 |
| 8 | 11 | 정상 복구 확인 | 3 | NORMAL 복귀 |

불연속 Counter가 수신되면 현재 Counter를 새로운 비교 기준으로 저장하고, 다음 Counter부터 정상 복구 확인을 다시 수행한다.

### 5.6 Alive Counter 순환 처리

Alive Counter 범위가 `0~15`인 경우 `15 → 0`은 정상적인 증가로 인정한다.

| 이전 Counter | 현재 Counter | 판단 |
|---:|---:|---|
| 14 | 15 | 정상 증가 |
| 15 | 0 | 정상 순환 |
| 0 | 1 | 정상 증가 |
| 15 | 1 | 불연속 Counter |

복구 과정에서 다음 Counter가 수신되면 정상 복구 3회로 인정한다.

```text
14 → 정상 복구 확인 1회
15 → 정상 복구 확인 2회
 0 → 정상 복구 확인 3회
   → NORMAL 복귀
```

## 6. 요구사항 변경

### 6.1 시스템 요구사항 Req_008 보완

**변경 전**

> Alive Counter 갱신 이상이 해제되고 정상적인 Alive Counter가 3회 연속 확인되면 시스템은 FAIL-SAFE 상태에서 NORMAL 상태로 복귀해야 한다.

**변경 후**

> Alive Counter 갱신 이상으로 FAIL-SAFE 상태에 진입한 경우, 시스템은 FAIL-SAFE 진입에 사용된 Alive Counter를 정상 복구 확인 횟수에서 제외해야 한다. FAIL-SAFE 진입 이후 정상적인 Alive Counter가 3회 연속 확인된 경우에만 NORMAL 상태로 복귀해야 한다.

### 6.2 SW 요구사항 변경

> **SWR-MON-006**: CanMonitor는 동일한 Alive Counter가 연속 수신되면 Alive Counter 갱신 이상 상태를 활성화해야 한다.

> **SWR-MON-007**: CanMonitor는 Alive Counter 갱신 이상 상태에 진입할 때 정상 복구 확인 횟수를 0으로 초기화해야 한다.

> **SWR-MON-008**: CanMonitor는 FAIL-SAFE 진입에 사용된 Alive Counter를 정상 복구 확인 횟수에 포함하지 않아야 한다.

> **SWR-MON-009**: CanMonitor는 FAIL-SAFE 진입 이후 현재 Alive Counter가 이전 Alive Counter보다 1 증가한 경우 정상 복구 확인 횟수를 1 증가시켜야 한다.

> **SWR-MON-010**: CanMonitor는 FAIL-SAFE 진입 이후 정상적인 Alive Counter가 3회 연속 확인된 경우에만 Alive Counter 갱신 이상 상태를 해제해야 한다.

> **SWR-MON-011**: 정상 복구 확인 중 동일하거나 불연속적인 Alive Counter가 수신되면 CanMonitor는 정상 복구 확인 횟수를 0으로 초기화해야 한다.

> **SWR-MON-012**: Alive Counter가 최댓값에 도달한 후 0으로 변경되는 경우 CanMonitor는 이를 정상적인 Counter 증가로 판단해야 한다.

## 7. 추적성 및 영향 분석

```text
DEF-INT-003
└─ CR-004
   └─ Req_008
      └─ SYS-F-008 / SYS-DES-008
         └─ SWR-MON-006–012
            └─ SWC-002 CanMonitor
               └─ App_CanMonitor.c
                  ├─ UT-MON-010–017
                  ├─ ITC-SW-018–021
                  └─ SYS-TC-REC-007–010
```

| 대상 | 영향 구분 | 조치 |
|---|---|---|
| Req_008 | 직접 영향 | 정상 복구 확인 시작 시점 명확화 |
| System Design | 직접 영향 | FAIL-SAFE 진입 후 복구 Counter 초기화 반영 |
| SWR-MON-006–012 | 신규·변경 | 복구 횟수 초기화 및 산정 기준 정의 |
| App_CanMonitor.c | 직접 영향 | FAIL-SAFE 진입 시 `recoveryCount = 0U` 적용 |
| App_SafetyPolicy.c | 간접 영향 | CanMonitor Flag 해제 시점이 한 주기 늦어짐 |
| App_ControlCalc.c | 회귀 영향 | FAIL-SAFE 유지 중 PWM 출력 차단 확인 |
| App_Pwm_Actuator.c | 회귀 영향 | FAIL-SAFE 유지 중 모터 구동 차단 확인 |
| AUTOSAR RTE Interface | 영향 없음 | 기존 조향값 및 Boolean Flag Interface 유지 |
| 단위시험 | 직접 영향 | 복구 시작 시점과 경계값 시험 추가 |
| 통합시험 | 직접 영향 | CanMonitor Flag와 SafetyPolicy 상태 전달 확인 |
| 시스템시험 | 회귀 영향 | 정상 복귀 시점 및 모터 출력 재개 확인 |

## 8. 설계 및 구현 변경

### 8.1 내부 상태 변수

| 변수 | 자료형 | 초기값 | 용도 |
|---|---|---:|---|
| `prevAliveCounter` | `uint8` | 0 | 이전 Alive Counter 저장 |
| `firstValid` | `boolean` | `FALSE` | 최초 유효 Counter 수신 여부 |
| `aliveFaultLatched` | `boolean` | `FALSE` | Alive Counter 갱신 이상 상태 유지 |
| `recoveryCount` | `uint8` | 0 | FAIL-SAFE 진입 이후 정상 Counter 확인 횟수 |

### 8.2 설정값

```c
#define ALIVE_COUNTER_MAX       (15U)
#define RECOVERY_VALID_LIMIT    (3U)
```

### 8.3 다음 Counter 계산

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

### 8.4 변경 전 로직

```c
if (aliveCounter == prevAliveCounter)
{
    aliveFaultLatched = TRUE;

    /*
     * FAIL-SAFE 진입값을 복구 확인값으로
     * 잘못 포함
     */
    recoveryCount = 1U;
}
```

### 8.5 변경 후 로직

```c
if (aliveCounter == prevAliveCounter)
{
    aliveFaultLatched = TRUE;

    /*
     * FAIL-SAFE 진입값은 복구 확인에서 제외
     */
    recoveryCount = 0U;
}
```

### 8.6 변경 로직 예시

```c
#include "Rte_SWC_CanMonitor.h"

#define STEER_ANGLE_MIN         (-512)
#define STEER_ANGLE_MAX         (511)
#define ALIVE_COUNTER_MAX       (15U)
#define RECOVERY_VALID_LIMIT    (3U)

static uint8 prevAliveCounter = 0U;
static uint8 recoveryCount = 0U;

static boolean firstValid = FALSE;
static boolean aliveFaultLatched = FALSE;

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
                 * 정상 복구 확인 횟수에서 제외한다.
                 */
                recoveryCount = 0U;
            }

            prevAliveCounter = aliveCounter;
        }
        else
        {
            if (aliveCounter ==
                GetNextAliveCounter(prevAliveCounter))
            {
                /*
                 * FAIL-SAFE 진입 이후 정상적으로
                 * 증가한 Counter만 복구 횟수에 포함
                 */
                recoveryCount++;
                prevAliveCounter = aliveCounter;

                if (recoveryCount >=
                    RECOVERY_VALID_LIMIT)
                {
                    aliveFaultLatched = FALSE;
                    recoveryCount = 0U;
                }
            }
            else
            {
                /*
                 * 정상적인 연속 증가가 끊긴 경우
                 * 복구 확인을 처음부터 다시 수행
                 */
                recoveryCount = 0U;
                prevAliveCounter = aliveCounter;
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
NORMAL 상태
     │
     ├─ Counter 정상 증가
     │      └─ NORMAL 유지
     │
     └─ 동일 Counter 수신
             ↓
        Alive Counter 갱신 이상 감지
             ↓
        FAIL-SAFE 진입
             ↓
        recoveryCount = 0
             ↓
        FAIL-SAFE 진입 Counter는 복구 횟수에서 제외
             ↓
        다음 Counter가 1 증가했는가?
             ├─ 아니요
             │     ├─ recoveryCount = 0
             │     └─ FAIL-SAFE 유지
             │
             └─ 예
                   └─ recoveryCount +1
                            ↓
                   정상 Counter 3회인가?
                       ├─ 아니요: FAIL-SAFE 유지
                       └─ 예: Alive Counter 이상 해제
                                      ↓
                                  NORMAL 복귀
```

## 10. SWC별 변경 책임

### 10.1 CanMonitor

- Alive Counter 갱신 이상 감지
- 동일 Counter 수신 시 Alive Counter 이상 상태 활성화
- FAIL-SAFE 진입 시 정상 복구 확인 횟수 초기화
- FAIL-SAFE 진입 Counter를 정상 복구 횟수에서 제외
- FAIL-SAFE 진입 이후 정상 Counter 3회 확인
- 연속 증가가 끊기면 정상 복구 확인 횟수 초기화
- 복구 조건 충족 전까지 SafetyPolicy에 Flag 유지

### 10.2 SafetyPolicy

- CanMonitor에서 전달된 Flag가 `TRUE`이면 FAIL-SAFE 유지
- CanMonitor가 정상 Counter 3회를 확인하여 Flag를 해제한 후 NORMAL 복귀
- Alive Counter와 복구 횟수를 직접 판단하지 않음

### 10.3 ControlCalc 및 PwmActuator

- FAIL-SAFE 상태가 유지되는 동안 PWM Duty와 방향 출력 차단
- SafetyPolicy가 NORMAL 상태로 복귀한 후 제어 출력 재개
- CR-003의 정상 복귀 기준값 동기화 로직과 함께 회귀시험 수행

## 11. 검증 변경 및 회귀시험

### 11.1 단위시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| UT-MON-010 | Counter `2 → 2` 수신 | FAIL-SAFE 진입, `recoveryCount = 0` |
| UT-MON-011 | FAIL-SAFE 후 Counter `3` 수신 | 복구 확인 1회, FAIL-SAFE 유지 |
| UT-MON-012 | FAIL-SAFE 후 `3 → 4` 수신 | 복구 확인 2회, FAIL-SAFE 유지 |
| UT-MON-013 | FAIL-SAFE 후 `3 → 4 → 5` 수신 | 복구 확인 3회, 이상 상태 해제 |
| UT-MON-014 | FAIL-SAFE 후 `3 → 4 → 4` 수신 | 복구 확인 횟수 0으로 초기화 |
| UT-MON-015 | FAIL-SAFE 후 `3 → 4 → 8` 수신 | 불연속 감지 및 복구 확인 횟수 초기화 |
| UT-MON-016 | `14 → 15 → 0` 정상 복구 수신 | Counter 순환을 포함한 정상 Counter 3회 인정 |
| UT-MON-017 | FAIL-SAFE 진입값을 복구 횟수에 포함하지 않는지 확인 | 진입 직후 `recoveryCount = 0` |

### 11.2 통합시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| ITC-SW-018 | Counter `2 → 2` 주입 | CanMonitor Flag 활성화 및 SafetyPolicy FAIL-SAFE 전환 |
| ITC-SW-019 | FAIL-SAFE 후 `3 → 4` 주입 | 정상 Counter 2회이므로 FAIL-SAFE 유지 |
| ITC-SW-020 | FAIL-SAFE 후 `3 → 4 → 5` 주입 | CanMonitor Flag 해제 및 SafetyPolicy NORMAL 복귀 |
| ITC-SW-021 | FAIL-SAFE 후 `3 → 4 → 4` 주입 | CanMonitor Flag 유지 및 FAIL-SAFE 유지 |

### 11.3 시스템 회귀시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| SYS-TC-REC-007 | CANoe에서 `2 → 2 → 3 → 4` 송신 | FAIL-SAFE 유지 및 모터 출력 차단 |
| SYS-TC-REC-008 | CANoe에서 `2 → 2 → 3 → 4 → 5` 송신 | `5` 수신 후 NORMAL 복귀 허용 |
| SYS-TC-REC-009 | CANoe에서 `2 → 2 → 3 → 4 → 4` 송신 | 복구 취소 및 FAIL-SAFE 유지 |
| SYS-TC-REC-010 | 정상 복귀 후 ControlCalc 및 PwmActuator 확인 | CR-003 기준값 동기화 후 정상 제어 재개 |

### 11.4 변경 전후 검증 결과

| 시험 입력 | 변경 전 결과 | 변경 후 기대 결과 |
|---|---|---|
| `2 → 2 → 3 → 4` | `4`에서 NORMAL 복귀 | FAIL-SAFE 유지 |
| `2 → 2 → 3 → 4 → 5` | 이미 NORMAL 상태 | `5`에서 NORMAL 복귀 |
| `2 → 2 → 3 → 4 → 4` | `4`에서 복귀 후 다시 FAIL-SAFE 가능 | 복구 횟수 초기화 및 FAIL-SAFE 유지 |

## 12. CR-003과의 관계

CR-003과 CR-004는 FAIL-SAFE 정상 복귀 과정에서 발생한 서로 다른 문제를 해결한다.

| 변경요청 | 발견 단계 | 문제 | 해결 방법 |
|---|---|---|---|
| CR-003 | 시스템검증 | FAIL-SAFE 동안 `prev_input_steer`가 0으로 갱신되어 복귀 순간 급격한 모터 출력 발생 | 복귀 첫 주기에 현재 조향 입력으로 제어 기준값 동기화 |
| CR-004 | SW 통합검증 | FAIL-SAFE 진입 Counter가 정상 복구 횟수에 포함되어 한 주기 일찍 복귀 | FAIL-SAFE 진입 시 복구 횟수를 0으로 초기화하고 이후 정상 Counter만 산정 |

두 변경이 모두 적용된 경우 정상 복귀 순서는 다음과 같다.

```text
동일 Alive Counter 수신
        ↓
FAIL-SAFE 진입
        ↓
FAIL-SAFE 진입 Counter는 복구 횟수에서 제외
        ↓
정상 Counter 3회 확인
3 → 4 → 5
        ↓
CanMonitor 이상 상태 해제
        ↓
SafetyPolicy NORMAL 복귀
        ↓
ControlCalc 복귀 첫 주기 기준값 동기화
        ↓
다음 제어 주기부터 정상 제어 재개
```

## 13. 형상 및 버전관리 계획

### 13.1 기준선

```text
BL-02:
FAIL-SAFE 진입 Counter를 정상 복구 횟수에 포함하여
2 → 3 → 4를 정상 Counter 3회로 판단하는 기존 로직

BL-03:
FAIL-SAFE 진입 시 복구 횟수를 0으로 초기화하고
FAIL-SAFE 이후 수신된 3 → 4 → 5를
정상 Counter 3회로 판단하는 변경 로직
```

### 13.2 Git 관리 예시

```bash
git checkout -b change/CR-004-recovery-counter-reset

git commit -m "CR-004 Clarify alive recovery count requirement"
git commit -m "CR-004 Reset recovery count on fail-safe entry"
git commit -m "CR-004 Add recovery counter boundary tests"
git commit -m "CR-004 Update integration verification"
git commit -m "CR-004 Update traceability matrix"

git tag -a BL-03 -m "CR-004 verified recovery counter baseline"
```

### 13.3 변경 대상 산출물

| 산출물 | 변경 내용 |
|---|---|
| `01_Requirements.md` | Req_008에 FAIL-SAFE 진입 Counter 제외 조건 추가 |
| `02_System_Design.md` | FAIL-SAFE 진입 시 복구 Counter 초기화 반영 |
| `03_SW_Requirements.md` | SWR-MON-006–012 추가 또는 변경 |
| `04_SW_Architecture_Design.md` | CanMonitor의 복구 Counter 관리 책임 반영 |
| `05_SW_Detailed_Design_Unit_Construction.md` | `recoveryCount` 초기화 및 증가 조건 반영 |
| `06_SW_Unit_Verification.md` | 복구 Counter 초기값 및 경계값 시험 추가 |
| `07_SW_Integration_Verification.md` | CanMonitor Flag 해제 시점 시험 추가 |
| `08_System_Verification.md` | 정상 복귀 및 출력 재개 회귀시험 추가 |
| `Traceability_Matrix.md` | DEF-INT-003, CR-004, 요구사항, 설계 및 시험 연결 |
| `App_CanMonitor.c` | FAIL-SAFE 진입 시 `recoveryCount = 0U` 적용 |

## 14. 반영 절차

1. SW 통합검증에서 확인된 조기 정상 복귀 현상을 DEF-INT-003으로 등록한다.
2. DEF-INT-003과 CR-004를 연결한다.
3. 요구사항, 설계, 구현 및 검증 항목에 대한 영향 분석을 수행한다.
4. Req_008에 FAIL-SAFE 진입 Counter 제외 조건을 명확하게 추가한다.
5. SW 요구사항과 상세설계에 복구 Counter 초기화 조건을 반영한다.
6. App_CanMonitor.c에서 FAIL-SAFE 진입 시 `recoveryCount`를 0으로 초기화한다.
7. 단위시험을 통해 FAIL-SAFE 진입값이 복구 횟수에 포함되지 않는지 확인한다.
8. 통합시험을 통해 정상 Counter `3 → 4`에서는 FAIL-SAFE가 유지되는지 확인한다.
9. 정상 Counter `3 → 4 → 5` 수신 후 CanMonitor Flag가 해제되는지 확인한다.
10. 시스템 회귀시험을 통해 CR-003 기준값 동기화 후 정상 제어가 재개되는지 확인한다.
11. Traceability Matrix와 관련 문서의 Version 및 변경이력을 갱신한다.
12. 검증 완료 후 CR-004를 Closed로 전환하고 BL-03 Tag를 생성한다.

---

본 문서는 기존의 정상 Counter 3회 확인 요구사항은 유지하면서, FAIL-SAFE 진입에 사용된 Alive Counter가 정상 복구 횟수에 포함되어 조기에 NORMAL 상태로 복귀하는 구현 결함을 SW 통합검증에서 발견하고 수정한 변경관리 시나리오다.
