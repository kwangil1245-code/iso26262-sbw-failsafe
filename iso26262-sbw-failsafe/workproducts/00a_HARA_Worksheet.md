# HARA 워크시트 (Hazard Analysis and Risk Assessment)

**Document ID**: STEER-00D-HARA
**ISO 26262 Reference**: Part 3 (Concept Phase, Hazard Analysis and Risk Assessment)
**ASPICE Reference**: SYS.2, SUP.10
**Version**: 1.6
**Date**: 2026-09-14
**Status**: Baseline (Educational Assessment)
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템
**Subtitle**: 조향 시스템 Hazard 분석 및 Safety Goal 도출

---

> 주요 Hazardous Event에 대한 S/E/C 평가와 Safety Goal 도출을 중심으로 정리한 문서입니다.

## 1. 목적 및 범위

본 문서는 AUTOSAR 기반 조향 오류 복구 및 진단 시스템에서 발생할 수 있는 주요 위험을 식별하고, Hazardous Event에 대한 Severity, Exposure, Controllability 평가를 통해 ASIL Candidate를 산정하며 Safety Goal을 도출하는 것을 목적으로 한다.

대상 시스템은 **조향 입력 → SteeringSensor → CAN → 조향 제어 SW → Actuator → Motor**로 이어지는 조향 데이터 및 제어 흐름이다. 조향 정보가 정상적으로 수신·갱신되지 않거나 유효하지 않은 조향 입력이 제어에 사용될 경우 발생할 수 있는 **비의도 조향 출력**을 주요 Hazard로 분석한다.

구체적인 통신 및 입력값 진단 조건, FAIL-SAFE 동작과 정상 복귀 조건은 후속 시스템 및 SW 요구사항·설계 단계에서 구체화한다.

본 문서의 ASIL 값은 교육 프로젝트 내부의 설계 우선순위 설정을 위한 후보 값이며, 실제 양산 차량의 공식 ISO 26262 ASIL 판정 결과를 의미하지 않는다.

## 2. S/E/C 평가 기준

| 항목                  | 레벨    | 의미                           |
| ------------------- | ----- | ---------------------------- |
| Severity (S)        | S0–S3 | 위험 발생 시 발생할 수 있는 피해의 심각도     |
| Exposure (E)        | E0–E4 | 해당 운행 상황에 노출될 가능성 또는 빈도      |
| Controllability (C) | C0–C3 | 위험 발생 후 운전자가 차량을 통제할 수 있는 정도 |

## 3. HARA 요약 워크시트

| HARA ID | Hazard    | Hazardous Event                                                           | Operational Situation  |  S  |  E  |  C  | ASIL Candidate | Safety Goal                                   |
| :-----: | --------- | ------------------------------------------------------------------------- | ---------------------- | :-: | :-: | :-: | :------------: | --------------------------------------------- |
|  HE-01  | 비의도 조향 출력 | 비정상 조향 정보가 제어에 사용되어 운전자 의도와 다른 조향 출력이 발생하고, 차량이 의도하지 않은 방향으로 움직여 충돌할 수 있음 | 차량 주행 중 조향 제어가 요구되는 상황 |  S3 |  E4 |  C3 |        D       | **SG-01:** 운전자 의도와 다른 비의도 조향 출력이 발생하지 않아야 한다. |

## 4. Safety Goal 요약

| Safety Goal ID | Safety Goal                        |
| -------------- | ---------------------------------- |
| SG-01          | 운전자 의도와 다른 비의도 조향 출력이 발생하지 않아야 한다. |

## 5. 후속 개발 단계와의 연계

SG-01을 기반으로 후속 개발 단계에서 조향 데이터의 수신·갱신 상태 감시, 입력값 유효성 확인, 이상 데이터의 제어 사용 방지, 안전 상태 전환 및 정상 복귀에 관한 요구사항을 구체화한다.

**개발 흐름**

`Hazardous Event → Safety Goal → Functional Safety Requirements → System/SW Requirements → Design → Implementation → Verification`

관련 산출물은 다음과 같다.

* `01_Requirements.md`: 기능안전 요구사항 및 시스템 요구사항 정의
* `02_System_Design.md`: 시스템 구조 및 인터페이스 정의
* `03_SW_Requirements.md`: SW 진단 및 안전 제어 요구사항 정의
* `04_SW_Architecture_Design.md`: SWC 구조 및 안전 메커니즘 설계
* `Traceability_Matrix.md`: HARA부터 검증까지의 양방향 추적성 관리
