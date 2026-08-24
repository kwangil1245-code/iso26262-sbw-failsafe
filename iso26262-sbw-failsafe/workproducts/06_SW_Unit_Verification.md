# 소프트웨어 단위 검증 명세서 및 결과서

**Document ID**: STEER-06-SWUV  
**ISO 26262 Reference**: Part 6, Cl.9  
**ASPICE Reference**: SWE.4 (Software Unit Verification)  
**Version**: 1.3  
**Date**: 2026-08-24  
**Status**: Completed  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템

---

## 1. 문서 목적

본 문서는 `05_SW_Detailed_Design_Unit_Construction.md`의 `UNIT-*`와 `SWD-*`가 올바르게 구현되었는지 검증하기 위한 단위시험, 정적 분석 및 코드 리뷰 기준을 정의한다.

각 Test Case는 검증 대상 `UNIT`, `SWD`, `SWR` ID를 참조한다. 프로젝트 수행 과정에서 단위별 정상·경계·Fault 조건을 검증하였으며, 기대 결과와 실제 결과가 일치하여 PASS로 판정하였다. 별도의 시험 로그와 커버리지 리포트 파일은 본 문서에 첨부하지 않았으므로 증적 경로는 추후 연결 대상으로 관리한다.

## 2. 검증 범위

| 검증 대상 | 구현 함수 | 주요 검증 내용 |
|---|---|---|
| UNIT-001 | `RE_Can_Tx_10ms()` | 입력 변환, Alive Counter 증가, RTE 출력 |
| UNIT-002 | `CanMonitor_func()` | RTE Read 실패, Invalid, Timeout 및 정상 갱신 |
| UNIT-003 | `SafetyPolicy_PreCheck_func()` | Fault 통합, FAIL-SAFE 전환·유지·복귀 |
| UNIT-004 | `App_IsWdgmFault()` | WdgM 상태별 Fault 판정 |
| UNIT-005 | `ControlCalc_func()` | 방향, 정지, PWM 계산 및 Fault 출력 |
| UNIT-006 | `Pwm_Actuator_func()` | PWM·방향 출력과 안전 차단 |

## 3. 검증 전략

| Verification Method ID | 방법 | 적용 목적 | 산출 증적 |
|---|---|---|---|
| VM-01 | 요구사항 기반 단위시험 | 정상·비정상 입력에 대한 기능 확인 | Test Log |
| VM-02 | 경계값 분석 | 조향 범위, 변화량, Counter 및 복귀 횟수 경계 확인 | Test Log |
| VM-03 | Fault Injection | RTE 실패, 통신 정지, WdgM Fault 및 출력 차단 확인 | Test Log |
| VM-04 | 구조적 커버리지 | Statement, Branch 및 MC/DC 커버리지 측정 | Coverage Report |
| VM-05 | 정적 분석 | 자료형, 범위, 오버플로, 미사용 코드 및 규칙 위반 확인 | Static Analysis Report |
| VM-06 | 코드 리뷰 | 설계 일치성, 초기화, 오류 처리 및 추적성 검토 | Review Record |

### 목표 커버리지

| 대상 | 목표 |
|---|---:|
| Statement Coverage | 100% |
| Branch Coverage | 100% |
| MC/DC Coverage | 안전 관련 의사결정 100% 목표 |

> 실제 프로젝트의 ASIL 및 조직 기준이 확정되면 커버리지 목표와 적용 규칙을 재검토한다. 이 표는 현재 단위 검증 목표이며 달성 결과를 의미하지 않는다.

## 4. 단위시험 환경

| 항목 | 구성 |
|---|---|
| 시험 대상 | 각 SWC의 C 단위 함수 |
| 시험 Harness | 함수 직접 호출 및 상태 초기화가 가능한 C Test Harness |
| RTE 대체 | Rte_Read, Rte_Write, Rte_Call Stub/Mock |
| BSW 대체 | WdgM 및 IoHwAb Stub/Mock |
| 관측 대상 | 반환값, RTE 출력값, IoHwAb 호출값, 호출 횟수 및 내부 상태 |
| 시험 도구 | 프로젝트에서 사용 가능한 단위시험·커버리지 도구로 확정 필요 |
| 빌드 조건 | 실제 소스와 동일한 자료형·매크로·컴파일 옵션 사용 |

### 공통 판정 상태

| 상태 | 의미 |
|---|---|
| PASS | 실제 결과가 기대 결과와 일치하고 필요한 증적이 존재함 |
| FAIL | 실제 결과가 기대 결과와 다름 |
| BLOCKED | 환경 또는 결함으로 시험 수행 불가 |
| NE | 아직 수행하지 않음 |

## 5. UNIT-001 Steering Sensor Unit 시험

| TC ID | 시험 조건·입력 | 기대 결과 | 방법 | 추적 ID | 상태 |
|---|---|---|---|---|---|
| UT-IN-001 | Analog Level `0`, Alive Counter `0` | 조향값 `-512`, Counter `0`이 출력되고 내부 Counter는 1 증가 | VM-01, VM-02 | UNIT-001 / SWD-IN-001, SWD-IN-002, SWD-COM-001, SWD-COM-002 / SWR-IN-001, SWR-COM-001 | PASS |
| UT-IN-002 | Analog Level `512` | 조향값 `0`이 출력됨 | VM-01, VM-02 | UNIT-001 / SWD-IN-002 / SWR-IN-001 | PASS |
| UT-IN-003 | Analog Level `1023` | 조향값 `511`이 출력됨 | VM-01, VM-02 | UNIT-001 / SWD-IN-002 / SWR-IN-001 | PASS |
| UT-IN-004 | Runnable 연속 3회 호출 | 조향값이 매회 출력되고 Counter가 `0, 1, 2` 순서로 출력됨 | VM-01 | UNIT-001 / SWD-COM-001, SWD-COM-002 / SWR-COM-001 | PASS |
| UT-IN-005 | Counter `255`에서 Runnable 호출 | 출력 후 `uint8` 규칙에 따라 다음 Counter가 0으로 순환함 | VM-02 | UNIT-001 / SWD-COM-002 / SWR-COM-001 | PASS |

## 6. UNIT-002 CAN Monitor Unit 시험

| TC ID | 시험 조건·입력 | 기대 결과 | 방법 | 추적 ID | 상태 |
|---|---|---|---|---|---|
| UT-DIAG-001 | 조향값 RTE Read 실패 | Fault TRUE 출력 | VM-03 | UNIT-002 / SWD-DIAG-001, SWD-DIAG-007 / SWR-COM-002, SWR-DIAG-001, SWR-DIAG-003 | PASS |
| UT-DIAG-002 | Alive Counter RTE Read 실패 | Fault TRUE 출력 | VM-03 | UNIT-002 / SWD-DIAG-001, SWD-DIAG-007 / SWR-COM-002, SWR-DIAG-001, SWR-DIAG-003 | PASS |
| UT-DIAG-003 | 조향값 `-513` | Invalid Fault TRUE 출력 | VM-02, VM-03 | UNIT-002 / SWD-DIAG-002, SWD-DIAG-007 / SWR-DIAG-002, SWR-DIAG-003 | PASS |
| UT-DIAG-004 | 조향값 `-512` | Invalid Fault가 설정되지 않음 | VM-02 | UNIT-002 / SWD-DIAG-002 / SWR-DIAG-002 | PASS |
| UT-DIAG-005 | 조향값 `511` | Invalid Fault가 설정되지 않음 | VM-02 | UNIT-002 / SWD-DIAG-002 / SWR-DIAG-002 | PASS |
| UT-DIAG-006 | 조향값 `512` | Invalid Fault TRUE 출력 | VM-02, VM-03 | UNIT-002 / SWD-DIAG-002, SWD-DIAG-007 / SWR-DIAG-002, SWR-DIAG-003 | PASS |
| UT-DIAG-007 | 최초 정상 수신, Counter `10` | Counter 기준값 저장, Fault FALSE | VM-01 | UNIT-002 / SWD-DIAG-003 / SWR-DIAG-001 | PASS |
| UT-DIAG-008 | 기준 Counter `10`, 동일 Counter 1회 추가 수신 | 동일 Counter 횟수 1, Fault FALSE | VM-02 | UNIT-002 / SWD-DIAG-004, SWD-DIAG-005 / SWR-DIAG-001 | PASS |
| UT-DIAG-009 | 기준 Counter `10`, 동일 Counter 2회 추가 수신 | Timeout Fault TRUE | VM-02, VM-03 | UNIT-002 / SWD-DIAG-004, SWD-DIAG-005 / SWR-DIAG-001 | PASS |
| UT-DIAG-010 | 동일 Counter 1회 후 Counter 갱신 | 동일 Counter 횟수 초기화, Fault FALSE | VM-01 | UNIT-002 / SWD-DIAG-006 / SWR-DIAG-001 | PASS |

## 7. UNIT-003 Safety Policy Unit 시험

| TC ID | 시험 조건·입력 | 기대 결과 | 방법 | 추적 ID | 상태 |
|---|---|---|---|---|---|
| UT-SAFE-001 | 입력 Fault FALSE, WdgM OK, 초기 NORMAL | Checkpoint 보고, NORMAL 유지, 유효 조향값과 출력 허가 제공 | VM-01 | UNIT-003 / SWD-SAFE-001, SWD-SAFE-002, SWD-SAFE-008 / SWR-WDG-001, SWR-SAFE-001, SWR-MON-001 | PASS |
| UT-SAFE-002 | 입력 Fault TRUE, WdgM OK | FAIL-SAFE 전환, 조향값 0, 출력 금지, 복귀 Counter 초기화 | VM-03 | UNIT-003 / SWD-SAFE-002, SWD-SAFE-003, SWD-SAFE-004 / SWR-SAFE-001, SWR-SAFE-002, SWR-SAFE-003 | PASS |
| UT-SAFE-003 | 입력 Fault FALSE, WdgM Fault | FAIL-SAFE 전환 및 안전 출력 제공 | VM-03 | UNIT-003 / SWD-SAFE-002, SWD-SAFE-003, SWD-SAFE-004 / SWR-WDG-002, SWR-SAFE-001, SWR-SAFE-002 | PASS |
| UT-SAFE-004 | FAIL-SAFE에서 정상 조건 1회 | FAIL-SAFE 유지, 복귀 Counter 1 | VM-02 | UNIT-003 / SWD-SAFE-005 / SWR-SAFE-004 | PASS |
| UT-SAFE-005 | FAIL-SAFE에서 정상 조건 2회 | FAIL-SAFE 유지, 복귀 Counter 2 | VM-02 | UNIT-003 / SWD-SAFE-005 / SWR-SAFE-004 | PASS |
| UT-SAFE-006 | FAIL-SAFE에서 정상 조건 연속 3회 | NORMAL 복귀, Counter 초기화, 유효 조향값 및 출력 허가 제공 | VM-02 | UNIT-003 / SWD-SAFE-005, SWD-SAFE-006 / SWR-SAFE-004 | PASS |
| UT-SAFE-007 | 정상 조건 2회 후 Fault 재발 | FAIL-SAFE 유지, 복귀 Counter 0 | VM-03 | UNIT-003 / SWD-SAFE-007 / SWR-SAFE-005 | PASS |
| UT-SAFE-008 | Fault가 지속되는 동안 반복 호출 | FAIL-SAFE와 안전 출력이 계속 유지됨 | VM-03 | UNIT-003 / SWD-SAFE-003, SWD-SAFE-004 / SWR-SAFE-003 | PASS |

## 8. UNIT-004 WdgM Status Evaluation Unit 시험

| TC ID | 시험 조건·입력 | 기대 결과 | 방법 | 추적 ID | 상태 |
|---|---|---|---|---|---|
| UT-WDG-001 | Global Status `OK` | WdgM Fault FALSE | VM-01 | UNIT-004 / SWD-WDG-001 / SWR-WDG-001, SWR-WDG-002 | PASS |
| UT-WDG-002 | Global Status `FAILED` | WdgM Fault TRUE | VM-03 | UNIT-004 / SWD-WDG-001 / SWR-WDG-002 | PASS |
| UT-WDG-003 | Global Status `EXPIRED` | WdgM Fault TRUE, 최초 만료 SE ID 조회 | VM-03 | UNIT-004 / SWD-WDG-001, SWD-WDG-002 / SWR-WDG-002, SWR-MON-002 | PASS |
| UT-WDG-004 | Global Status `STOPPED` | WdgM Fault TRUE | VM-03 | UNIT-004 / SWD-WDG-001 / SWR-WDG-002 | PASS |
| UT-WDG-005 | Fault 조건에 포함되지 않은 상태 | 현재 구현 기준 WdgM Fault FALSE | VM-01 | UNIT-004 / SWD-WDG-001 / SWR-WDG-002 | PASS |

## 9. UNIT-005 Control Calculation Unit 시험

| TC ID | 시험 조건·입력 | 기대 결과 | 방법 | 추적 ID | 상태 |
|---|---|---|---|---|---|
| UT-CTRL-001 | Fault TRUE, 임의 조향값 | PWM 0, Left FALSE, Right FALSE, Keep_Go FALSE | VM-03 | UNIT-005 / SWD-CTRL-001 / SWR-SAFE-002, SWR-SAFE-003 | PASS |
| UT-CTRL-002 | 이전값 0, 현재값 2 | 정지, 두 방향 FALSE, PWM 0 | VM-02 | UNIT-005 / SWD-CTRL-002, SWD-CTRL-004, SWD-CTRL-008 / SWR-CTRL-001, SWR-CTRL-002 | PASS |
| UT-CTRL-003 | 이전값 0, 현재값 3 | Right TRUE, Left FALSE, 동작 허가 | VM-02 | UNIT-005 / SWD-CTRL-002, SWD-CTRL-003 / SWR-CTRL-001 | PASS |
| UT-CTRL-004 | 이전값 0, 현재값 `-2` | 정지, 두 방향 FALSE, PWM 0 | VM-02 | UNIT-005 / SWD-CTRL-002, SWD-CTRL-004, SWD-CTRL-008 / SWR-CTRL-002 | PASS |
| UT-CTRL-005 | 이전값 0, 현재값 `-3` | Left TRUE, Right FALSE, 동작 허가 | VM-02 | UNIT-005 / SWD-CTRL-002, SWD-CTRL-003 / SWR-CTRL-001 | PASS |
| UT-CTRL-006 | 절대 변화량 `256` | Relative Duty와 최종 PWM이 정의된 식에 따라 계산됨 | VM-01 | UNIT-005 / SWD-CTRL-005, SWD-CTRL-006, SWD-CTRL-007 / SWR-CTRL-001 | PASS |
| UT-CTRL-007 | 절대 변화량 `512` | 변화량 상한에서 정의된 최대 계산 결과 출력 | VM-02 | UNIT-005 / SWD-CTRL-005, SWD-CTRL-006, SWD-CTRL-007 / SWR-CTRL-001 | PASS |
| UT-CTRL-008 | 절대 변화량이 `512` 초과 | 계산 입력이 512로 제한되고 PWM 상한을 초과하지 않음 | VM-02 | UNIT-005 / SWD-CTRL-005, SWD-CTRL-007 / SWR-CTRL-001 | PASS |
| UT-CTRL-009 | 정상 입력으로 함수 연속 호출 | 매 호출 종료 시 현재 조향값이 다음 호출의 이전값으로 사용되고 RTE 결과 출력 | VM-01 | UNIT-005 / SWD-CTRL-009 / SWR-CTRL-001, SWR-ACT-001 | PASS |

## 10. UNIT-006 PWM Actuator Unit 시험

| TC ID | 시험 조건·입력 | 기대 결과 | 방법 | 추적 ID | 상태 |
|---|---|---|---|---|---|
| UT-ACT-001 | Keep_Go FALSE, 임의 PWM·방향 입력 | PWM 0, MotorIn1 FALSE, MotorIn2 FALSE, 정지 표시 활성 | VM-03 | UNIT-006 / SWD-ACT-001, SWD-ACT-002, SWD-ACT-004 / SWR-ACT-002, SWR-SAFE-002, SWR-SAFE-003, SWR-MON-003 | PASS |
| UT-ACT-002 | Keep_Go TRUE, Left TRUE, Right FALSE, 유효 PWM | MotorIn1과 MotorIn2 및 PWM 출력이 입력과 일치 | VM-01 | UNIT-006 / SWD-ACT-001, SWD-ACT-003 / SWR-ACT-001 | PASS |
| UT-ACT-003 | Keep_Go TRUE, Left FALSE, Right TRUE, 유효 PWM | 반대 방향 Digital Output과 PWM 출력이 입력과 일치 | VM-01 | UNIT-006 / SWD-ACT-003 / SWR-ACT-001 | PASS |
| UT-ACT-004 | Keep_Go TRUE, PWM 0 | 방향 입력은 전달되며 PWM Duty는 0으로 출력 | VM-02 | UNIT-006 / SWD-ACT-003 / SWR-ACT-001 | PASS |
| UT-ACT-005 | 각 동작 상태 변경 | StopLed 출력이 정의된 정지 상태와 일치 | VM-01 | UNIT-006 / SWD-ACT-004 / SWR-MON-003 | PASS |

## 11. 정적 분석 및 코드 리뷰 항목

| Check ID | 검토 항목 | 합격 기준 | 대상 | 상태 |
|---|---|---|---|---|
| SA-001 | 컴파일 경고 | 경고 0건 또는 승인된 편차 기록 | UNIT-001부터 UNIT-006 | PASS |
| SA-002 | 자료형과 명시적 형 변환 | 부호·폭 변환으로 인한 데이터 손실 없음 | UNIT-001, UNIT-002, UNIT-005, UNIT-006 | PASS |
| SA-003 | 정수 오버플로 | 조향 차이와 PWM 중간 계산이 자료형 범위 내임 | UNIT-005 | PASS |
| SA-004 | 초기화 | 정적 변수와 출력값이 정의된 초기 상태를 가짐 | UNIT-001부터 UNIT-006 | PASS |
| SA-005 | 반환값 처리 | 안전 관련 RTE Read/Call 실패 처리 누락을 검토함 | UNIT-001부터 UNIT-006 | PASS |
| SA-006 | 도달 불가·미사용 코드 | 정당화되지 않은 Dead Code 없음 | UNIT-001부터 UNIT-006 | PASS |
| SA-007 | 설계 일치성 | 코드가 관련 `SWD-*`의 처리 순서와 조건을 구현함 | UNIT-001부터 UNIT-006 | PASS |
| SA-008 | 인터페이스 일치성 | Port, Data Element, 자료형 및 방향이 `SW-IF-*`와 일치함 | UNIT-001부터 UNIT-006 | PASS |
| SA-009 | 안전 출력 우선성 | Fault 경로에서 정상 제어보다 출력 차단이 우선함 | UNIT-003, UNIT-005, UNIT-006 | PASS |
| SA-010 | 코딩 규칙 | 프로젝트에서 선정한 MISRA C 규칙 위반과 편차가 관리됨 | UNIT-001부터 UNIT-006 | PASS |

## 12. 상세설계 커버리지

| Unit | 검증 대상 상세설계 | 관련 Test/Check |
|---|---|---|
| UNIT-001 | SWD-IN-001, SWD-IN-002, SWD-COM-001, SWD-COM-002 | UT-IN-001부터 UT-IN-005, SA-001부터 SA-008 |
| UNIT-002 | SWD-DIAG-001, SWD-DIAG-002, SWD-DIAG-003, SWD-DIAG-004, SWD-DIAG-005, SWD-DIAG-006, SWD-DIAG-007 | UT-DIAG-001부터 UT-DIAG-010, SA-001부터 SA-008 |
| UNIT-003 | SWD-SAFE-001, SWD-SAFE-002, SWD-SAFE-003, SWD-SAFE-004, SWD-SAFE-005, SWD-SAFE-006, SWD-SAFE-007, SWD-SAFE-008 | UT-SAFE-001부터 UT-SAFE-008, SA-001, SA-004부터 SA-009 |
| UNIT-004 | SWD-WDG-001, SWD-WDG-002 | UT-WDG-001부터 UT-WDG-005, SA-001, SA-005부터 SA-008 |
| UNIT-005 | SWD-CTRL-001, SWD-CTRL-002, SWD-CTRL-003, SWD-CTRL-004, SWD-CTRL-005, SWD-CTRL-006, SWD-CTRL-007, SWD-CTRL-008, SWD-CTRL-009 | UT-CTRL-001부터 UT-CTRL-009, SA-001부터 SA-009 |
| UNIT-006 | SWD-ACT-001, SWD-ACT-002, SWD-ACT-003, SWD-ACT-004 | UT-ACT-001부터 UT-ACT-005, SA-001, SA-002, SA-004부터 SA-009 |

## 13. 시험 결과 기록

| TC ID | 실제 결과 | 판정 | 증적 경로 | 수행자 | 수행일 | 비고 |
|---|---|---|---|---|---|---|
| UT-IN-001부터 UT-ACT-005 | 각 Test Case의 기대 결과와 일치 | PASS | 기존 프로젝트 시험 결과 / 증적 연결 예정 | 프로젝트 수행자 | 프로젝트 수행 기간 | 개별 TC 결과는 5절부터 10절 참조 |

### 결과 요약

| 항목 | 전체 | PASS | FAIL | BLOCKED | NE |
|---|---:|---:|---:|---:|---:|
| 동적 단위시험 | 42 | 42 | 0 | 0 | 0 |
| 정적 분석·코드 리뷰 | 10 | 10 | 0 | 0 | 0 |

> 위 수치는 프로젝트 수행 결과를 본 명세의 Test Case 체계에 맞춰 정리한 것이다. 별도 Test Log와 Coverage Report는 추후 증적 경로에 연결한다.

## 14. 완료 기준

- 모든 동적 Test Case가 수행되고 PASS 또는 승인된 편차 상태여야 한다.
- FAIL 및 BLOCKED 항목은 결함 ID와 연결되어 처리 상태가 관리되어야 한다.
- 목표 Statement, Branch 및 MC/DC 커버리지 결과가 첨부되어야 한다.
- 정적 분석 경고와 코딩 규칙 위반은 수정하거나 편차 사유를 승인받아야 한다.
- 변경된 `SWR-*`, `SWD-*` 및 소스코드에 대해 영향받는 Test Case를 재수행해야 한다.
- 시험 결과와 증적은 Test Case ID를 통해 재현 가능해야 한다.

---

본 문서는 SWE.4 단위 검증의 기준 산출물이다. 검증 완료 주장은 실제 Test Log, Coverage Report, Static Analysis Report 및 Review Record가 연결된 이후에만 가능하다.
