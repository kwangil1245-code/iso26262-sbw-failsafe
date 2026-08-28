# HARA 워크시트 (Hazard Analysis and Risk Assessment)

**Document ID**: STEER-00D-HARA  
**ISO 26262 Reference**: Part 3 (Concept Phase, Hazard Analysis and Risk Assessment)  
**ASPICE Reference**: SYS.2, SUP.10  
**Version**: 1.4  
**Date**: 2026-08-27  
**Status**: Baseline (Educational Assessment)  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**Subtitle**: 조향 시스템 Hazard 분석 및 Safety Goal 도출  

---

> 주요 Hazardous Event에 대한 S/E/C 평가와 Safety Goal 도출을 중심으로 정리한 문서입니다.

## 1. 목적 및 범위

본 문서는 AUTOSAR 기반 조향 오류 복구 및 진단 시스템에서 발생 가능한 주요 위험 상황을 식별하고, 각 Hazardous Event에 대한 Severity, Exposure, Controllability 평가를 통해 ASIL Candidate를 산정하며 Safety Goal을 도출하는 것을 목적으로 한다.

대상 시스템은 **조향 입력 → SteeringSensor → CAN → 조향 제어 SW → Actuator → Motor**로 이어지는 전체 조향 데이터 및 제어 흐름을 기준으로 하며, 다음 세 가지 위험 관점을 주요 분석 대상으로 한다.

* 조향 데이터 갱신 이상
* 비정상 조향 입력
* 조향 제어 관련 SW 실행 이상

본 문서에서는 구체적인 진단 메커니즘, AUTOSAR 모듈, Fault 판정 조건, FAIL-SAFE 동작 및 복귀 조건 등의 구현 방법은 정의하지 않으며, 해당 내용은 후속 시스템 및 SW 요구사항·설계 단계에서 구체화한다.

본 문서의 ASIL 값은 교육 프로젝트 내부의 설계 우선순위 설정을 위한 후보 값이며, 실제 양산 차량의 공식 ISO 26262 ASIL 판정 결과를 의미하지 않는다.

## 2. S/E/C 평가 기준

| 항목                  | 레벨    | 의미                                   |
| ------------------- | ----- | ------------------------------------ |
| Severity (S)        | S0–S3 | 위험 발생 시 운전자 및 탑승자에게 발생할 수 있는 피해의 심각도 |
| Exposure (E)        | E0–E4 | 해당 운행 상황에 노출될 가능성 또는 빈도              |
| Controllability (C) | C0–C3 | 위험 발생 후 운전자가 차량을 통제할 수 있는 정도         |

## 3. HARA 요약 워크시트

| HARA ID | Hazardous Event                                                     | Operational Situation                              | S  | E  | C  | ASIL Candidate  | Safety Goal                                                        |
| ------- | ------------------------------------------------------------------- | -------------------------------------------------- | -- | -- | -- | --------------- | ------------------------------------------------------------------ |
| HC-01   | 조향 데이터의 갱신 이상으로 오래된 조향 입력이 사용되어 운전자 의도와 다른 조향이 발생함                  | 차량 주행 중 입력 ECU 또는 통신 이상으로 조향 데이터가 정상적으로 갱신되지 않는 상황 | S3 | E4 | C3 | D | **SG-01:** 조향 데이터의 갱신 이상을 감지하고 위험한 조향 출력이 발생하지 않도록 해야 한다.          |
| HC-02   | 비정상적인 조향 입력이 정상 입력으로 사용되어 운전자 의도와 다른 조향이 발생함                        | 차량 주행 중 센서 오류 등으로 정상 범위를 벗어난 조향 입력이 발생하는 상황        | S3 | E4 | C3 | D | **SG-02:** 비정상적인 조향 입력을 감지하고 해당 입력으로 인해 위험한 조향 출력이 발생하지 않도록 해야 한다. |
| HC-03   | 조향 제어 관련 SW 실행 이상으로 오래된 데이터 또는 비정상적인 제어 결과가 사용되어 운전자 의도와 다른 조향이 발생함 | 차량 주행 중 ECU 내부의 조향 제어 기능 실행에 이상이 발생하는 상황           | S3 | E3 | C3 | D | **SG-03:** 조향 제어 관련 SW 실행 이상을 감지하고 위험한 조향 출력이 발생하지 않도록 해야 한다.      |

## 4. Safety Goal 요약

| Safety Goal ID | Safety Goal                                             |
| -------------- | ------------------------------------------------------- |
| SG-01          | 조향 데이터의 갱신 이상을 감지하고 위험한 조향 출력이 발생하지 않도록 해야 한다.          |
| SG-02          | 비정상적인 조향 입력을 감지하고 해당 입력으로 인해 위험한 조향 출력이 발생하지 않도록 해야 한다. |
| SG-03          | 조향 제어 관련 SW 실행 이상을 감지하고 위험한 조향 출력이 발생하지 않도록 해야 한다.      |

## 5. 후속 개발 단계와의 연계

본 HARA에서 도출한 Safety Goal은 후속 개발 단계에서 기능 안전 요구사항, 시스템 요구사항 및 SW 요구사항으로 구체화한다.

**개발 흐름**

`Hazardous Event → Safety Goal → Functional/System Requirements → System Design → SW Requirements → SW Architecture → Implementation → Verification`

관련 산출물은 다음과 같다.

* `01_Requirements.md`: Safety Goal을 기반으로 기능 안전 요구사항 및 시스템 요구사항 구체화
* `02_System_Design.md`: 시스템 구조, 기능 할당 및 인터페이스 정의
* `03_SW_Requirements.md`: SW가 만족해야 할 구체적인 진단 및 안전 제어 요구사항 정의
* `04_SW_Architecture_Design.md`: SWC 구조, 인터페이스 및 안전 메커니즘 설계
* `05_SW_Detailed_Design_Unit_Construction.md`: 상세 로직 및 구현 정의
* `06_SW_Unit_Verification.md`: SW 단위 수준 검증
* `07_SW_Integration_Verification.md`: SW 통합 및 인터페이스 검증
* `08_System_Verification.md`: 시스템 수준 Fault Injection 및 안전 동작 검증
* `Traceability_Matrix.md`: HARA부터 시스템 검증까지의 양방향 추적성 관리

---

본 문서의 승인 상태와 ASIL Candidate는 교육 프로젝트 내부 Baseline을 의미하며, ISO 26262 인증 또는 실제 양산 차량의 공식 안전 승인을 의미하지 않는다.
