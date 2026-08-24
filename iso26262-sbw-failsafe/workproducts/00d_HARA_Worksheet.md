# HARA 워크시트 (Hazard Analysis and Risk Assessment)

**Document ID**: STEER-00D-HARA  
**ISO 26262 Reference**: Part 3 (Concept Phase, Hazard Analysis and Risk Assessment)  
**ASPICE Reference**: SYS.2, SUP.10  
**Version**: 1.3  
**Date**: 2026-08-24  
**Status**: Baseline (Educational Assessment)  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**Subtitle**: CAN 입력 감시, WdgM 실행 감시 및 FAIL-SAFE 기반 조향 출력 차단

---

> 제출용 정리본: 주요 위험 이벤트, 안전 목표, 구현 조건 및 대표 검증 근거만 유지한 문서입니다.

## 1. 목적 및 범위

본 문서는 AUTOSAR 기반 조향 오류 복구 및 진단 시스템에서 발생 가능한 주요 위험 상황에 대한 S/E/C 평가, ASIL 후보, 안전 목표 및 대표 검증 관점을 정리한다.

대상 시스템은 **조향 입력 → SteeringSensor → CAN → CanMonitor → SafetyPolicy → ControlCalc → PWM Actuator**로 이어지는 데이터 흐름을 기준으로 하며, CAN 메시지 Timeout, 조향각 Invalid, Alive Counter 이상, WdgM 기반 실행 이상, FAIL-SAFE 전환 및 복귀, PWM·방향 출력 이상을 주요 분석 대상으로 한다.

본 문서의 ASIL 값은 교육 프로젝트 내부의 설계 우선순위 설정을 위한 후보 값이며, 실제 양산 차량의 공식 ISO 26262 ASIL 판정 결과를 의미하지 않는다.

## 2. S/E/C 평가 기준(요약)

| 항목 | 레벨 | 의미 |
|---|---|---|
| Severity (S) | S0–S3 | 위험 발생 시 운전자 및 탑승자에게 발생할 수 있는 피해의 심각도 |
| Exposure (E) | E0–E4 | 해당 운행 상황에 노출될 가능성 또는 빈도 |
| Controllability (C) | C0–C3 | 위험 발생 후 운전자가 차량을 통제할 수 있는 정도 |

## 3. HARA 요약 워크시트

| HARA ID | 관련 Req | Hazardous Event (요약) | Operational Situation | S | E | C | ASIL Candidate | 안전 목표 | 구현 조건(참고) | 대표 검증 |
|---|---|---|---|---|---|---|---|---|---|---|
| HC-01 | Req_002, Req_003, Req_006, Req_007 | CAN 메시지 미수신을 감지하지 못해 이전 조향 명령이 계속 사용됨 | 주행 중 입력 ECU 또는 CAN 통신 이상으로 조향 데이터 갱신 중단 | S3 | E4 | C3 | D (Provisional) | **SG-01:** 조향 CAN 데이터가 정상적으로 갱신되지 않는 경우 Timeout을 검출하고 조향 출력을 안전 상태로 전환해야 한다. | 입력 ECU 10 ms 주기 송신, 동일 Alive Counter 2회 이상 시 Fault | `UT-DIAG-007–010`, `ITC-SW-005`, `SYS-TC-FLT-001`, `SYS-TC-FLT-004` |
| HC-02 | Req_001, Req_004, Req_006, Req_007 | 비정상 조향각이 정상 입력으로 처리되어 운전자 의도와 다른 조향 출력이 발생함 | 주행 중 센서 오류, 센서 단선 또는 허용 범위를 벗어난 조향 데이터 발생 | S3 | E4 | C3 | D (Provisional) | **SG-02:** 허용 범위를 벗어난 조향 입력은 Invalid로 판정하고 해당 입력을 조향 출력에 사용하지 않아야 한다. | CAN Data Received Event에서 입력 유효성 검사 | `UT-DIAG-003–006`, `ITC-SW-004`, `ITC-SW-006`, `SYS-TC-FLT-002–004` |
| HC-03 | Req_005–Req_007 | SW Runnable 또는 Task 실행 이상을 감지하지 못해 오래된 데이터나 비정상 제어 출력이 지속됨 | ECU 내부에서 조향 제어 Runnable 또는 Task의 주기·실행 순서 이상 발생 | S3 | E3 | C3 | D (Provisional) | **SG-03:** 조향 제어 관련 SW 실행 이상을 WdgM으로 감지하고 이상 발생 시 FAIL-SAFE 상태로 전환해야 한다. | WdgM Global Status 기반 Fault 판정 | `UT-WDG-001–005`, `ITC-SW-007–009`, `SYS-TC-FLT-005` |
| HC-04 | Req_006, Req_007 | 입력 또는 내부 실행 이상이 발생했음에도 FAIL-SAFE 전환이 수행되지 않아 조향 출력이 지속됨 | 차량 주행 중 Timeout, Invalid 또는 WdgM Fault 발생 | S3 | E4 | C3 | D (Provisional) | **SG-04:** 조향 입력 또는 내부 실행 Fault 발생 시 NORMAL에서 FAIL-SAFE로 전환하고 PWM 출력을 제한하거나 차단해야 한다. | Fault 전달 이벤트에 따라 SafetyPolicy-ControlCalc-Actuator 순차 실행 | `UT-SAFE-002–003`, `ITC-SW-010–015`, `SYS-TC-FLT-001–005` |
| HC-05 | Req_008 | 일시적인 정상 입력만으로 FAIL-SAFE가 해제되어 불안정한 상태에서 조향 출력이 재활성화됨 | 간헐적 CAN 장애 또는 센서 오류 후 정상 데이터가 일시적으로 수신됨 | S3 | E3 | C2 | C (Provisional) | **SG-05:** FAIL-SAFE 상태는 연속 정상 조건이 확인된 경우에만 NORMAL 상태로 복귀해야 한다. | 정상 입력 3회 연속 확인 후 복귀 | `UT-SAFE-004–007`, `SYS-TC-REC-001–003` |
| HC-06 | Req_009, Req_010 | PWM Duty 또는 좌·우 방향 출력 오류로 운전자 의도와 다른 조향 출력이 발생함 | 정상 조향 입력 중 ControlCalc 계산 오류 또는 Actuator 출력 경로 이상 발생 | S3 | E4 | C3 | D (Provisional) | **SG-06:** 조향 방향 및 PWM 출력은 검증된 입력과 SafetyPolicy 상태를 기반으로 생성되어야 하며 Fault 상태에서는 PWM 출력을 차단해야 한다. | 제어 Cycle 내 적용 | `UT-CTRL-001–009`, `UT-ACT-001–005`, `ITC-SW-012–015`, `SYS-TC-OUT-001–003` |

## 4. 연계 검증 문서

HARA의 안전 목표와 구현 조건은 아래 요구사항 및 검증 문서와 함께 유지한다.

- `01_Requirements.md`: 시스템 요구사항과 Safety Goal의 상위 추적성
- `05_SW_Unit_Verification.md`: CanMonitor, SafetyPolicy, ControlCalc 단위검증
- `06_SW_Integration_Verification.md`: Host Stub/Mock 기반 SWC 인터페이스 및 데이터 흐름 검증
- `07_System_Verification.md`: 입력 ECU 기능 확인 및 CAPL Fault Injection 기반 시스템 검증
- `Traceability_Matrix.md`: HARA부터 시스템시험까지의 최종 양방향 추적성 SSOT

---

본 문서의 승인 상태와 ASIL Candidate는 교육 프로젝트 내부 Baseline을 의미하며, ISO 26262 인증 또는 실제 양산 차량의 공식 안전 승인을 의미하지 않는다.
