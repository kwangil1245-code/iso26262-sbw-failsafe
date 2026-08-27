# 변경관리 시나리오: FAIL-SAFE 해제 시 급격한 모터 동작 방지

**Change Request ID**: CR-003  
**Title**: FAIL-SAFE 정상 복귀 시 제어 기준값 동기화  
**Version**: 0.3  
**Date**: 2026-08-27  
**Status**: Planned  
**Project**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템

---

## 1. 변경 배경 및 목적

시스템검증에서 FAIL-SAFE 상태가 해제된 직후 모터가 한쪽 방향으로 급격하게 동작하는 현상을 확인하였다.

FAIL-SAFE 상태에서는 모터 출력을 차단하기 위해 SafetyPolicy가 ControlCalc에 조향값 `0`과 출력 금지 Flag를 전달한다. 그러나 기존 ControlCalc는 출력 금지 상태에서도 함수 마지막에 `prev_input_steer = input_steer`를 실행한다.

따라서 FAIL-SAFE가 유지되는 동안 `prev_input_steer`가 `0`으로 변경된다. 이후 정상 복귀 조건이 충족되어 실제 조향 입력이 다시 전달되면, ControlCalc는 실제 조향 입력과 `0`의 차이를 조향 변화량으로 판단하여 큰 PWM Duty와 방향 출력을 생성할 수 있다.

본 변경의 목적은 정상 복귀 첫 주기에 현재 조향 입력을 제어 기준값으로 동기화하여, FAIL-SAFE 해제 순간 발생할 수 있는 급격한 모터 동작을 방지하는 것이다.

## 2. 변경요청

| 항목 | 내용 |
|---|---|
| 발견 단계 | 시스템검증 |
| 관련 시험 | SYS-TC-REC-004 |
| 변경 전 | FAIL-SAFE 상태에서도 `prev_input_steer`를 전달값 `0`으로 갱신 |
| 문제 현상 | 정상 복귀 시 실제 조향 입력과 `0`의 차이를 변화량으로 계산하여 모터가 급격히 동작 |
| 변경 후 | FAIL-SAFE 동안 기준값 갱신을 중지하고 정상 복귀 첫 주기에 현재 입력으로 기준값만 동기화 |
| 복귀 첫 주기 출력 | PWM Duty `0`, Left/Right `FALSE`, 모터 구동 금지 |
| 제어 재개 시점 | 기준값 동기화가 완료된 다음 주기 |
| 변경 유형 | 정상 복귀 시 제어 연속성 및 출력 안정화 로직 변경 |

## 3. 변경 전 문제 재현

### 3.1 재현 절차

1. 정상 상태에서 조향 제어를 수행한다.
2. FAIL-SAFE 상태로 진입한다.
3. SafetyPolicy가 ControlCalc에 조향값 `0`과 출력 금지 Flag를 전달한다.
4. ControlCalc의 `prev_input_steer`가 `0`으로 갱신된다.
5. 정상 복귀 조건이 충족되어 실제 조향 입력 `300`이 다시 전달된다.
6. ControlCalc가 `300 - 0`을 조향 변화량으로 계산한다.

### 3.2 변경 전 동작

| 실행 주기 | 상태 | 전달 조향값 | `prev_input_steer` | 계산 변화량 | 출력 |
|---|---|---:|---:|---:|---|
| T0 | NORMAL | 250 | 250 | 정상 변화량 | 정상 제어 |
| T1 | FAIL-SAFE | 0 | 0으로 갱신 | 계산하지 않음 | PWM 0 |
| T2 | FAIL-SAFE | 0 | 0으로 유지 | 계산하지 않음 | PWM 0 |
| T3 | 정상 복귀 | 300 | 0 | 300 | 큰 PWM 및 방향 출력 가능 |

정상 복귀 자체는 허용된 상태 전이지만, 내부 기준값이 안전 상태에서 사용한 대체값 `0`으로 바뀌어 제어 출력의 연속성이 깨진다.

## 4. 원인 분석

문제의 직접적인 원인은 ControlCalc의 무조건적인 이전 입력 갱신이다.

```c
prev_input_steer = input_steer;
```

FAIL-SAFE 상태에서 `input_steer`는 실제 조향 입력이 아니라 출력 차단을 위해 SafetyPolicy가 전달한 대체값 `0`이다. 기존 로직은 이 값을 정상 제어 입력과 동일하게 취급하여 다음 제어 주기의 기준값으로 저장하였다.

즉, 안전 출력 차단을 위한 대체값과 정상 제어 계산을 위한 기준값을 구분하지 않은 것이 원인이다.

## 5. 요구사항 변경

### 5.1 시스템 요구사항 Req_008 보완

**변경 전**

> 정상 복귀 조건이 충족되면 시스템은 FAIL-SAFE 상태에서 NORMAL 상태로 복귀해야 한다.

**변경 후**

> 정상 복귀 조건이 충족되면 시스템은 FAIL-SAFE 상태에서 NORMAL 상태로 복귀해야 한다. 정상 복귀 첫 제어 주기에는 현재 조향 입력을 ControlCalc의 기준값으로 동기화하고 모터 출력을 비활성 상태로 유지해야 한다. 동기화가 완료된 다음 주기부터 입력 변화량에 따라 정상 제어를 수행해야 한다.

### 5.2 하위 SW 요구사항

> **SWR-CTRL-006**: ControlCalc는 출력 금지 Flag가 활성화된 동안 `prev_input_steer`를 SafetyPolicy가 전달한 대체 조향값 `0`으로 갱신하지 않아야 한다.

> **SWR-CTRL-007**: ControlCalc는 출력 금지 Flag가 해제된 첫 주기에 현재 조향 입력을 `prev_input_steer`에 저장하고 PWM Duty와 방향 출력을 비활성 상태로 유지해야 한다.

> **SWR-CTRL-008**: ControlCalc는 정상 복귀 기준값 동기화가 완료된 다음 주기부터 현재 입력과 `prev_input_steer`의 차이를 이용하여 방향과 PWM Duty를 계산해야 한다.

## 6. 추적성 및 영향 분석

```text
CR-003
└─ Req_008
   └─ SYS-F-008 / SYS-DES-008
      └─ SWR-CTRL-006 / SWR-CTRL-007 / SWR-CTRL-008
         └─ SWC-004 ControlCalc / SW-IF-005
            └─ App_ControlCalc.c
               ├─ UT-CTRL-006–009
               ├─ ITC-SW-017
               └─ SYS-TC-REC-004–006
```

| 대상 | 영향 구분 | 조치 |
|---|---|---|
| Req_008 | 직접 영향 | 정상 복귀 시 기준값 동기화 조건 추가 |
| System Design | 직접 영향 | FAIL-SAFE 해제 후 동기화 주기 추가 |
| SWR-CTRL-006–008 | 신규 | 기준값 보존·동기화·제어 재개 요구사항 정의 |
| App_SafetyPolicy.c | 회귀 영향 | 정상 복귀 조건과 출력 금지 Flag 해제 시점 확인 |
| App_ControlCalc.c | 직접 영향 | FAIL-SAFE 이력 저장 및 복귀 첫 주기 처리 추가 |
| App_Pwm_Actuator.c | 회귀 영향 | 동기화 주기에 PWM과 방향 출력이 차단되는지 확인 |
| AUTOSAR RTE Interface | 영향 없음 | 기존 조향값 및 Boolean Flag Interface 유지 |
| 단위·통합·시스템시험 | 직접 영향 | 정상 복귀 경계와 급출력 방지 시험 추가 |

## 7. 설계 및 구현 변경

### 7.1 변경 원칙

- FAIL-SAFE 상태에서는 PWM Duty와 방향 출력을 기존과 같이 차단한다.
- FAIL-SAFE 상태에서 전달되는 대체 조향값 `0`은 `prev_input_steer`에 저장하지 않는다.
- 출력 금지 Flag가 `TRUE`에서 `FALSE`로 바뀐 첫 주기를 정상 복귀 동기화 주기로 판단한다.
- 동기화 주기에는 현재 실제 조향 입력을 `prev_input_steer`에 저장하되 모터는 구동하지 않는다.
- 다음 주기부터 동기화된 기준값과 현재 입력의 차이로 제어 출력을 계산한다.
- 기존 RTE Interface는 변경하지 않고 ControlCalc 내부 상태만 추가한다.

### 7.2 변경 로직 예시

```c
static sint16 prev_input_steer = 0;
static boolean was_failsafe = FALSE;

if (input_flag == TRUE)
{
    AbsoluteDutyCycle = 0;
    go_flag = FALSE;
    Left = FALSE;
    Right = FALSE;
    was_failsafe = TRUE;
}
else if (was_failsafe == TRUE)
{
    /* 정상 복귀 첫 주기: 현재 입력을 기준값으로 동기화 */
    prev_input_steer = input_steer;

    AbsoluteDutyCycle = 0;
    go_flag = FALSE;
    Left = FALSE;
    Right = FALSE;
    was_failsafe = FALSE;
}
else
{
    steer_diff = input_steer - prev_input_steer;

    /* 기존 방향 및 PWM Duty 계산 */

    prev_input_steer = input_steer;
}
```

기존 함수 마지막의 무조건 갱신 코드는 제거하고 정상 제어 또는 정상 복귀 동기화 조건에서만 `prev_input_steer`를 갱신한다.

## 8. 변경 후 예상 동작

| 실행 주기 | 상태 | 전달 조향값 | 기준값 처리 | 계산 변화량 | 출력 |
|---|---|---:|---|---:|---|
| T0 | NORMAL | 250 | `prev = 250` | 정상 변화량 | 정상 제어 |
| T1 | FAIL-SAFE | 0 | 기존 기준값 보존 | 계산하지 않음 | PWM 0 |
| T2 | FAIL-SAFE | 0 | 기존 기준값 보존 | 계산하지 않음 | PWM 0 |
| T3 | 정상 복귀 첫 주기 | 300 | `prev = 300`으로 동기화 | 계산하지 않음 | PWM 0 |
| T4 | NORMAL | 302 | 동기화 기준 사용 | 2 | 정상 제어 |

정상 복귀 중 실제 조향 위치가 변경되어도 첫 주기에는 해당 위치를 새로운 기준으로 설정하므로 복귀 순간의 급격한 모터 동작을 방지할 수 있다.

## 9. 검증 변경 및 회귀시험

### 9.1 단위시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| UT-CTRL-006 | FAIL-SAFE 상태에서 조향값 0 전달 | `prev_input_steer`를 0으로 갱신하지 않고 출력 차단 |
| UT-CTRL-007 | FAIL-SAFE 해제 첫 주기에 조향값 300 전달 | `prev_input_steer = 300`, PWM 0, 방향 출력 없음 |
| UT-CTRL-008 | 동기화 다음 주기에 조향값 302 전달 | 변화량 2를 기준으로 정상 제어 |
| UT-CTRL-009 | 동기화 다음 주기에 조향값 310 전달 | 변화량 10을 기준으로 방향과 PWM 계산 |

### 9.2 통합·시스템시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| ITC-SW-017 | SafetyPolicy Flag 해제 → ControlCalc → PwmActuator | 복귀 첫 주기 PWM 0 및 방향 출력 차단 |
| SYS-TC-REC-004 | FAIL-SAFE 중 실제 조향 입력을 300으로 이동 후 정상 복귀 | 복귀 순간 모터 급동작 없음 |
| SYS-TC-REC-005 | 정상 복귀 후 조향 입력을 점진적으로 변경 | 동기화 다음 주기부터 입력 변화량에 따라 정상 동작 |
| SYS-TC-REC-006 | 동기화 주기 직후 다시 FAIL-SAFE 진입 | 출력 즉시 차단 및 복귀 이력 재설정 |

회귀시험은 정상 조향 출력, FAIL-SAFE 진입 시 PWM·방향 차단, 정상 복귀 조건 및 복귀 후 정상 제어 재개를 대상으로 한다.

## 10. 형상 및 버전관리 계획

### 10.1 기준선

```text
BL-01: FAIL-SAFE 상태에서 prev_input_steer가 0으로 갱신되는 기존 로직
BL-02: 정상 복귀 첫 주기에 현재 조향 입력을 기준값으로 동기화하는 로직
```

### 10.2 Git 관리 예시

```bash
git checkout -b change/CR-003-bumpless-recovery
git commit -m "CR-003 Update fail-safe recovery requirements"
git commit -m "CR-003 Synchronize control reference after fail-safe"
git commit -m "CR-003 Add recovery transition tests"
git commit -m "CR-003 Update traceability matrix"
git tag -a BL-02 -m "CR-003 verified fail-safe recovery baseline"
```

### 10.3 변경 대상 산출물

| 산출물 | 변경 내용 |
|---|---|
| `01_Requirements.md` | Req_008에 정상 복귀 동기화 조건 추가 |
| `02_System_Design.md` | FAIL-SAFE 해제 후 동기화 동작 반영 |
| `03_SW_Requirements.md` | SWR-CTRL-006–008 추가 |
| `04_SW_Architecture_Design.md` | ControlCalc의 정상 복귀 처리 책임 반영 |
| `05_SW_Detailed_Design_Unit_Construction.md` | `was_failsafe`와 기준값 갱신 조건 반영 |
| `06_SW_Unit_Verification.md` | 기준값 보존·동기화 단위시험 추가 |
| `07_SW_Integration_Verification.md` | Flag 해제부터 Actuator 출력까지 연계시험 추가 |
| `08_System_Verification.md` | 정상 복귀 순간 급출력 방지 시험 추가 |
| `Traceability_Matrix.md` | CR-003과 영향 요구사항·설계·시험 연결 |
| `App_ControlCalc.c` | 무조건 기준값 갱신 제거 및 복귀 동기화 로직 추가 |

## 11. 반영 절차

1. 시스템검증 결과와 재현 조건을 CR-003에 등록한다.
2. Req_008 및 하위 SW 요구사항의 변경 영향을 검토한다.
3. 시스템설계, SW 요구사항, 아키텍처 및 상세설계를 상위에서 하위 순서로 수정한다.
4. ControlCalc의 기준값 보존 및 정상 복귀 동기화 로직을 구현한다.
5. 단위·통합·시스템시험과 회귀시험을 수행한다.
6. 시험 결과와 수정 Commit을 관련 Test Case ID에 연결한다.
7. `Traceability_Matrix.md`와 변경 대상 문서의 Version 및 변경이력을 갱신한다.
8. 검증 완료 후 CR-003을 Closed로 전환하고 `BL-02` Tag를 생성한다.

---

본 문서는 시스템검증에서 발견된 정상 복귀 순간의 급격한 모터 동작을 요구사항, 설계, 구현, 시험 및 형상관리까지 전개하는 포트폴리오용 변경관리 시나리오다.
