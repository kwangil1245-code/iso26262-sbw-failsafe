# 변경관리 시나리오: Alive Counter 정상 연속성 확인 후 FAIL-SAFE 복귀

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

그러나 FAIL-SAFE 진입 후 이전 값과 다른 Alive Counter가 한 번 수신되면 통신이 정상화된 것으로 판단하여 즉시 NORMAL 상태로 복귀하였다.

예를 들어 Alive Counter가 `2 → 2`로 수신되면 FAIL-SAFE 상태로 진입하지만, 다음 주기에 `3`이 한 번 수신되는 즉시 정상 상태로 복귀하였다.

```text
2 → 2 → FAIL-SAFE
        ↓
        3 → NORMAL 즉시 복귀
```

이 방식은 Alive Counter가 한 번 갱신된 것만으로 통신이 안정적으로 복구되었다고 판단하기 때문에, 일시적인 정상 수신이나 간헐적인 통신 이상 조건에서 FAIL-SAFE와 NORMAL 상태가 반복적으로 전환될 수 있다.

본 변경의 목적은 Alive Counter가 정상적인 순서로 연속 증가하는 것을 확인한 후에만 FAIL-SAFE 상태를 해제하여, 정상 복귀 판정의 신뢰성을 높이는 것이다.

## 2. 변경요청

| 항목 | 내용 |
|---|---|
| 발견 단계 | SW 통합검증 |
| 관련 결함 | DEF-INT-003 |
| 관련 시험 | ITC-SW-018 |
| 변경 전 | 동일 Counter 수신으로 FAIL-SAFE 진입 후 갱신된 Counter 1회 수신 시 즉시 복귀 |
| 문제점 | 일시적인 Counter 갱신만으로 NORMAL 상태에 복귀할 수 있음 |
| 변경 후 | Alive Counter가 이전 값보다 1씩 증가하는 상태가 3회 연속 확인된 경우에만 복귀 |
| 복구 기준 | FAIL-SAFE 진입 후 최초 수신된 Counter를 복구 기준값으로 설정 |
| 불연속 수신 시 | 복구 횟수를 0으로 초기화하고 현재 Counter를 새로운 기준값으로 설정 |
| Counter 순환 | 최댓값 다음에 0이 수신되는 경우 정상 증가로 인정 |
| 변경 유형 | FAIL-SAFE 정상 복귀 판정 로직 변경 |

## 3. 변경 전 문제 재현

### 3.1 재현 절차

1. Alive Counter `2`를 정상 수신한다.
2. 다음 주기에 동일한 Alive Counter `2`를 수신한다.
3. CanMonitor가 Alive Counter 갱신 이상을 감지한다.
4. SafetyPolicy가 FAIL-SAFE 상태로 전환한다.
5. 다음 주기에 Alive Counter `3`을 수신한다.
6. CanMonitor가 갱신된 Counter를 정상으로 판단한다.
7. SafetyPolicy가 즉시 NORMAL 상태로 복귀한다.

### 3.2 변경 전 동작

| 수신 순서 | Alive Counter | CanMonitor 판단 | 시스템 상태 |
|---:|---:|---|---|
| 1 | 2 | 정상 수신 | NORMAL |
| 2 | 2 | 동일 Counter 수신 | FAIL-SAFE |
| 3 | 3 | Counter 갱신 | NORMAL 즉시 복귀 |

Alive Counter `3`이 한 번 수신되었지만, 이후 `4`, `5`, `6`과 같이 정상적으로 증가할지는 아직 확인되지 않은 상태이다.

따라서 Counter가 한 번 변경된 것만으로 통신 복구를 확정하는 것은 충분하지 않다.

### 3.3 간헐적 통신 이상 조건

| 수신 순서 | Alive Counter | 변경 전 상태 | 문제점 |
|---:|---:|---|---|
| 1 | 2 | NORMAL | 정상 |
| 2 | 2 | FAIL-SAFE | 갱신 이상 감지 |
| 3 | 3 | NORMAL | 1회 갱신만으로 즉시 복귀 |
| 4 | 3 | FAIL-SAFE | 다시 갱신 이상 발생 |
| 5 | 4 | NORMAL | 다시 즉시 복귀 |

위 조건에서는 시스템 상태가 다음과 같이 반복 전환될 수 있다.

```text
NORMAL → FAIL-SAFE → NORMAL → FAIL-SAFE → NORMAL
```

이로 인해 모터 출력 차단과 허용이 반복될 가능성이 있다.

## 4. 원인 분석

기존 CanMonitor 로직은 현재 Alive Counter가 이전 값과 같은지만 확인하였다.

```c
if (aliveCounter == prevAliveCounter)
{
    flag = TRUE;
}
else
{
    flag = FALSE;
}
```

이 구조에서는 이전 값과 다른 Counter가 수신되기만 하면 정상으로 판단한다.

따라서 다음과 같은 비정상적인 Counter 변화도 정상으로 처리될 수 있다.

```text
2 → 2 → FAIL-SAFE
2 → 7 → 다른 값이므로 정상 판정 가능
2 → 3 → 한 번 갱신됐으므로 즉시 정상 판정
```

문제의 원인은 Alive Counter의 단순 변경 여부만 확인하고, 다음 조건을 확인하지 않은 것이다.

- 이전 Counter보다 정확히 1 증가했는가
- 정상 증가가 연속적으로 유지되는가
- Counter 최댓값 이후 0으로 정상 순환하는가

## 5. 변경 내용

### 5.1 정상 복귀 기준

Alive Counter 갱신 이상으로 FAIL-SAFE 상태에 진입한 경우 다음 절차로 복구 여부를 판단한다.

1. FAIL-SAFE 진입 후 최초 수신된 Alive Counter를 복구 기준값으로 저장한다.
2. 다음 Counter가 기준값보다 1 증가했는지 확인한다.
3. 정상 증가하면 복구 확인 횟수를 1 증가시킨다.
4. 정상 증가가 3회 연속 확인되면 Alive Counter 이상 상태를 해제한다.
5. 정상 증가 확인 중 예상값과 다른 Counter가 수신되면 복구 횟수를 0으로 초기화한다.
6. 불연속으로 수신된 현재 Counter를 새로운 기준값으로 설정하고 복구 확인을 다시 시작한다.

### 5.2 변경 후 정상 복귀 예시

| 수신 순서 | Alive Counter | 복구 판단 | 복구 횟수 | 시스템 상태 |
|---:|---:|---|---:|---|
| 1 | 2 | 정상 기준값 | - | NORMAL |
| 2 | 2 | 동일 Counter | 0 | FAIL-SAFE 진입 |
| 3 | 3 | 복구 기준값 설정 | 0 | FAIL-SAFE 유지 |
| 4 | 4 | 1 증가 확인 | 1 | FAIL-SAFE 유지 |
| 5 | 5 | 1 증가 확인 | 2 | FAIL-SAFE 유지 |
| 6 | 6 | 1 증가 확인 | 3 | NORMAL 복귀 |

즉, FAIL-SAFE 진입 후 `3`, `4`, `5`, `6`이 순서대로 수신되어야 정상 증가 3회를 확인할 수 있다.

### 5.3 복구 중 불연속 Counter 수신

| 수신 순서 | Alive Counter | 복구 판단 | 복구 횟수 | 시스템 상태 |
|---:|---:|---|---:|---|
| 1 | 2 | 정상 기준값 | - | NORMAL |
| 2 | 2 | 동일 Counter | 0 | FAIL-SAFE 진입 |
| 3 | 3 | 복구 기준값 설정 | 0 | FAIL-SAFE 유지 |
| 4 | 4 | 1 증가 확인 | 1 | FAIL-SAFE 유지 |
| 5 | 7 | 예상값 5와 불일치 | 0 | FAIL-SAFE 유지 |
| 6 | 8 | 7에서 1 증가 | 1 | FAIL-SAFE 유지 |
| 7 | 9 | 8에서 1 증가 | 2 | FAIL-SAFE 유지 |
| 8 | 10 | 9에서 1 증가 | 3 | NORMAL 복귀 |

Counter `7`이 수신된 시점에는 복구 횟수를 초기화하고, `7`을 새로운 기준값으로 사용한다.

### 5.4 Alive Counter 순환 처리

Alive Counter의 범위가 `0~15`인 경우 `15 → 0`은 정상적인 증가로 인정한다.

| 이전 Counter | 현재 Counter | 판단 |
|---:|---:|---|
| 14 | 15 | 정상 증가 |
| 15 | 0 | 정상 순환 |
| 0 | 1 | 정상 증가 |
| 15 | 1 | 불연속 증가 |

## 6. 요구사항 변경

### 6.1 시스템 요구사항 Req_008 변경

**변경 전**

> 통신 상태가 정상화되면 시스템은 FAIL-SAFE 상태에서 NORMAL 상태로 복귀해야 한다.

**변경 후**

> Alive Counter 갱신 이상으로 FAIL-SAFE 상태에 진입한 경우, 시스템은 Alive Counter가 정상적인 순서로 3회 연속 증가한 것이 확인된 후에만 NORMAL 상태로 복귀해야 한다. 연속 증가 확인 중 Counter 순서가 불연속적인 경우 복구 확인을 다시 시작해야 한다.

### 6.2 SW 요구사항 변경

> **SWR-MON-006**: CanMonitor는 동일한 Alive Counter가 연속 수신되면 Alive Counter 갱신 이상 상태를 활성화해야 한다.

> **SWR-MON-007**: Alive Counter 갱신 이상 상태에서 CanMonitor는 최초 수신된 Alive Counter를 정상 복귀 확인의 기준값으로 저장해야 한다.

> **SWR-MON-008**: CanMonitor는 현재 Alive Counter가 이전 Alive Counter보다 1 증가한 경우 정상 증가로 판단해야 한다.

> **SWR-MON-009**: CanMonitor는 Alive Counter의 정상 증가가 3회 연속 확인된 경우에만 Alive Counter 갱신 이상 상태를 해제해야 한다.

> **SWR-MON-010**: 정상 증가 확인 중 예상값과 다른 Alive Counter가 수신되면 CanMonitor는 복구 확인 횟수를 0으로 초기화하고, 현재 Counter를 새로운 복구 기준값으로 저장해야 한다.

> **SWR-MON-011**: Alive Counter가 최댓값에 도달한 후 0으로 변경되는 경우 CanMonitor는 이를 정상 증가로 판단해야 한다.

## 7. 추적성 및 영향 분석

```text
DEF-INT-003
└─ CR-004
   └─ Req_008
      └─ SYS-F-008 / SYS-DES-008
         └─ SWR-MON-006–011
            └─ SWC-002 CanMonitor
               └─ App_CanMonitor.c
                  ├─ UT-MON-010–016
                  ├─ ITC-SW-018–020
                  └─ SYS-TC-REC-007–009
```

| 대상 | 영향 구분 | 조치 |
|---|---|---|
| Req_008 | 직접 영향 | Alive Counter 정상 복귀 조건 구체화 |
| System Design | 직접 영향 | FAIL-SAFE 복귀 조건에 Counter 연속성 확인 추가 |
| SWR-MON-006–011 | 신규·변경 | 진입, 복구, 불연속 및 순환 조건 정의 |
| App_CanMonitor.c | 직접 영향 | 복구 기준값과 연속 증가 Counter 추가 |
| App_SafetyPolicy.c | 간접 영향 | CanMonitor Flag가 해제될 때까지 FAIL-SAFE 유지 |
| App_ControlCalc.c | 회귀 영향 | FAIL-SAFE 유지 중 PWM 출력이 계속 차단되는지 확인 |
| App_Pwm_Actuator.c | 회귀 영향 | FAIL-SAFE 상태에서 구동 출력이 차단되는지 확인 |
| AUTOSAR RTE Interface | 영향 없음 | 기존 Boolean Flag Interface 유지 |
| 단위시험 | 직접 영향 | Counter 증가, 불연속, 순환 경계값 시험 추가 |
| 통합시험 | 직접 영향 | CanMonitor에서 SafetyPolicy까지 상태 전달 확인 |
| 시스템시험 | 직접 영향 | CANoe Fault 주입 및 정상 복귀 시나리오 변경 |

## 8. 설계 및 구현 변경

### 8.1 내부 상태 변수

| 변수 | 자료형 | 초기값 | 용도 |
|---|---|---:|---|
| `prevAliveCounter` | `uint8` | 0 | 이전 Alive Counter 저장 |
| `firstValid` | `boolean` | `FALSE` | 최초 유효 Counter 수신 여부 |
| `aliveFaultLatched` | `boolean` | `FALSE` | Alive Counter 갱신 이상 상태 유지 |
| `recoveryBaseValid` | `boolean` | `FALSE` | 복구 기준값 설정 여부 |
| `recoveryCount` | `uint8` | 0 | 정상 증가 연속 확인 횟수 |

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

### 8.4 변경 로직 예시

```c
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

    Std_ReturnType retAlive;
    Std_ReturnType retAngle;

    retAlive =
        Rte_Read_Project_SSU_SteerInfo_SSU_AliveCounter(&aliveCounter);

    retAngle =
        Rte_Read_Project_SSU_SteerInfo_SSU_SteerAngle(&angle);

    if ((retAlive != RTE_E_OK) || (retAngle != RTE_E_OK))
    {
        aliveFaultLatched = TRUE;
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
                aliveFaultLatched = TRUE;
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
                 * FAIL-SAFE 진입 후 최초 Counter를
                 * 정상 복귀 확인의 기준값으로 설정
                 */
                prevAliveCounter = aliveCounter;
                recoveryBaseValid = TRUE;
                recoveryCount = 0U;
            }
            else if (aliveCounter ==
                     GetNextAliveCounter(prevAliveCounter))
            {
                recoveryCount++;
                prevAliveCounter = aliveCounter;

                if (recoveryCount >= RECOVERY_VALID_LIMIT)
                {
                    aliveFaultLatched = FALSE;
                    recoveryBaseValid = FALSE;
                    recoveryCount = 0U;
                }
            }
            else
            {
                /*
                 * 예상값과 다른 경우 현재 Counter를
                 * 새로운 복구 기준값으로 설정
                 */
                prevAliveCounter = aliveCounter;
                recoveryCount = 0U;
            }
        }
    }

    if (aliveFaultLatched == TRUE)
    {
        flag = TRUE;
    }

    if ((angle < -512) || (angle > 511))
    {
        flag = TRUE;
    }

    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Steer_info(angle);
    (void)Rte_Write_P_CanMonitorToSafetyPolicy_Flag(flag);
}
```

## 9. SWC별 변경 책임

### 9.1 CanMonitor

- Alive Counter 갱신 이상 감지
- Alive Counter 이상 상태 유지
- FAIL-SAFE 진입 후 최초 Counter를 복구 기준값으로 저장
- 정상 증가 3회 연속 확인
- 불연속 Counter 수신 시 복구 횟수 초기화
- Counter 최댓값 이후 0으로의 순환 처리
- 복구 조건 충족 전까지 Fault Flag 유지

### 9.2 SafetyPolicy

- CanMonitor에서 전달된 Flag가 `TRUE`이면 FAIL-SAFE 유지
- CanMonitor가 복구 조건을 확인하여 Flag를 해제한 후 NORMAL 복귀 허용
- Alive Counter의 증가 여부를 직접 판단하지 않음

### 9.3 ControlCalc 및 PwmActuator

- FAIL-SAFE 상태가 유지되는 동안 PWM Duty와 방향 출력 차단
- 정상 복귀 후 제어 출력 재개
- CR-003의 기준값 동기화 로직과 함께 회귀시험 수행

## 10. 검증 변경 및 회귀시험

### 10.1 단위시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| UT-MON-010 | Counter `2 → 2` 수신 | Alive Counter 이상 상태 활성화 |
| UT-MON-011 | FAIL-SAFE 후 `3 → 4 → 5` 수신 | 정상 증가 2회이므로 FAIL-SAFE 유지 |
| UT-MON-012 | FAIL-SAFE 후 `3 → 4 → 5 → 6` 수신 | 정상 증가 3회 확인 후 이상 상태 해제 |
| UT-MON-013 | 복구 중 `3 → 4 → 7` 수신 | 복구 횟수 0으로 초기화, 7을 새 기준값으로 설정 |
| UT-MON-014 | `7 → 8 → 9 → 10` 수신 | 정상 증가 3회 후 이상 상태 해제 |
| UT-MON-015 | `14 → 15 → 0 → 1` 수신 | Counter 순환을 포함한 정상 증가 3회 인정 |
| UT-MON-016 | `14 → 15 → 1` 수신 | 불연속 Counter로 판단하고 복구 횟수 초기화 |

### 10.2 통합시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| ITC-SW-018 | Counter `2 → 2` 주입 | CanMonitor Flag 활성화 및 SafetyPolicy FAIL-SAFE 전환 |
| ITC-SW-019 | FAIL-SAFE 후 정상 증가 2회 | CanMonitor Flag 유지 및 SafetyPolicy FAIL-SAFE 유지 |
| ITC-SW-020 | FAIL-SAFE 후 정상 증가 3회 | CanMonitor Flag 해제 및 SafetyPolicy 정상 복귀 허용 |

### 10.3 시스템시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| SYS-TC-REC-007 | CANoe에서 `2 → 2 → 3` 송신 | `3` 수신 후에도 FAIL-SAFE 유지 |
| SYS-TC-REC-008 | CANoe에서 `2 → 2 → 3 → 4 → 5 → 6` 송신 | `6` 수신 후 NORMAL 복귀 |
| SYS-TC-REC-009 | CANoe에서 `2 → 2 → 3 → 4 → 7 → 8 → 9 → 10` 송신 | 불연속 시 복구 초기화 후 `10`에서 NORMAL 복귀 |

### 10.4 회귀시험

회귀시험은 다음 항목을 대상으로 수행한다.

- 정상 Alive Counter 수신 시 오검출 여부
- 동일 Alive Counter 수신 시 FAIL-SAFE 진입
- 조향 입력 Invalid 발생 시 FAIL-SAFE 진입
- FAIL-SAFE 상태에서 PWM Duty 0 유지
- FAIL-SAFE 상태에서 Left/Right 방향 출력 차단
- Alive Counter 정상 순환 처리
- 정상 복귀 후 CR-003의 제어 기준값 동기화
- 기준값 동기화 다음 주기부터 정상 제어 재개

## 11. CR-003과의 관계

CR-003과 CR-004는 서로 다른 문제를 해결한다.

| 변경요청 | 문제 | 해결 방법 |
|---|---|---|
| CR-003 | FAIL-SAFE 중 `prev_input_steer`가 0으로 변경되어 복귀 순간 급격한 모터 출력 발생 | 정상 복귀 첫 주기에 현재 조향 입력으로 제어 기준값 동기화 |
| CR-004 | Alive Counter가 한 번 갱신되면 FAIL-SAFE가 즉시 해제됨 | Alive Counter가 1씩 증가하는 상태를 3회 연속 확인한 후 복귀 |

두 변경이 모두 적용된 경우 정상 복귀 순서는 다음과 같다.

```text
Alive Counter 이상 감지
        ↓
FAIL-SAFE 진입 및 출력 차단
        ↓
최초 Counter를 복구 기준값으로 설정
        ↓
Alive Counter 정상 증가 3회 확인
        ↓
CanMonitor 이상 상태 해제
        ↓
SafetyPolicy NORMAL 복귀 허용
        ↓
ControlCalc 현재 조향값 기준 동기화
        ↓
다음 주기부터 정상 제어 재개
```

## 12. 형상 및 버전관리 계획

### 12.1 기준선

```text
BL-02:
Alive Counter가 한 번 갱신되면 즉시 정상 복귀하는 기존 로직

BL-03:
Alive Counter가 1씩 증가하는 상태를 3회 연속 확인한 후 정상 복귀하는 로직
```

### 12.2 Git 관리 예시

```bash
git checkout -b change/CR-004-alive-recovery

git commit -m "CR-004 Update alive counter recovery requirements"
git commit -m "CR-004 Add consecutive alive recovery logic"
git commit -m "CR-004 Add alive recovery unit tests"
git commit -m "CR-004 Add integration and system tests"
git commit -m "CR-004 Update traceability matrix"

git tag -a BL-03 -m "CR-004 verified alive counter recovery baseline"
```

### 12.3 변경 대상 산출물

| 산출물 | 변경 내용 |
|---|---|
| `01_Requirements.md` | Req_008에 Alive Counter 정상 증가 3회 조건 반영 |
| `02_System_Design.md` | FAIL-SAFE 정상 복귀 조건과 상태 전이 변경 |
| `03_SW_Requirements.md` | SWR-MON-006–011 추가 |
| `04_SW_Architecture_Design.md` | CanMonitor의 복구 판정 책임 반영 |
| `05_SW_Detailed_Design_Unit_Construction.md` | 복구 Counter와 상태 변수 및 처리 흐름 추가 |
| `06_SW_Unit_Verification.md` | 연속 증가, 불연속 및 순환 시험 추가 |
| `07_SW_Integration_Verification.md` | CanMonitor에서 SafetyPolicy까지 복구 전달 시험 추가 |
| `08_System_Verification.md` | CANoe 기반 정상 복귀 시나리오 변경 |
| `Traceability_Matrix.md` | DEF-INT-003, CR-004, 요구사항, 설계 및 시험 연결 |
| `App_CanMonitor.c` | Alive Counter 이상 상태 유지 및 정상 증가 3회 복구 로직 추가 |

## 13. 반영 절차

1. 통합검증에서 확인된 즉시 정상 복귀 현상을 DEF-INT-003으로 등록한다.
2. DEF-INT-003과 CR-004를 연결한다.
3. 변경 대상 요구사항과 SWC 및 시험 항목에 대한 영향 분석을 수행한다.
4. Req_008과 SWR-MON-006–011을 변경하거나 추가한다.
5. 시스템설계, SW 아키텍처 및 상세설계를 갱신한다.
6. App_CanMonitor.c에 Alive Counter 정상 증가 3회 확인 로직을 구현한다.
7. 단위시험을 통해 증가, 불연속 및 순환 조건을 검증한다.
8. 통합시험을 통해 CanMonitor와 SafetyPolicy의 상태 전달을 검증한다.
9. CANoe 기반 시스템시험으로 FAIL-SAFE 유지 및 정상 복귀 시점을 확인한다.
10. CR-003을 포함한 정상 복귀 전체 경로의 회귀시험을 수행한다.
11. Traceability Matrix와 관련 문서의 Version 및 변경이력을 갱신한다.
12. 검증 완료 후 CR-004를 Closed로 전환하고 BL-03 Tag를 생성한다.

---

본 문서는 Alive Counter가 한 번 갱신되면 FAIL-SAFE 상태가 즉시 해제되는 문제를 SW 통합검증에서 발견하고, 요구사항·설계·구현·시험 및 형상관리까지 변경 영향을 전개한 포트폴리오용 변경관리 시나리오다.
