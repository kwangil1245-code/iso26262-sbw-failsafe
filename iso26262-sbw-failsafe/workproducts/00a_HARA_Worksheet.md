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

> 비정상 조향 정보로 인해 발생할 수 있는 차량 수준의 위험을 식별하고, Hazardous Event에 대한 S/E/C 평가와 Safety Goal 도출을 중심으로 정리한 문서입니다.

## 1. 목적 및 범위

본 문서는 AUTOSAR 기반 조향 오류 복구 및 진단 시스템에서 비정상 조향 정보로 인해 발생할 수 있는 차량 수준의 위험을 식별하고, 해당 Hazardous Event에 대한 Severity, Exposure, Controllability 평가를 통해 ASIL Candidate를 산정하며 Safety Goal을 도출하는 것을 목적으로 한다.

대상 시스템은 **조향 입력 → SteeringSensor → CAN → 조향 제어 SW → Actuator → Motor**로 이어지는 조향 데이터 및 제어 흐름이다. 본 HARA에서는 조향 정보의 수신 중단·갱신 이상과 유효하지 않은 조향 입력이 제어에 사용될 경우 발생할 수 있는 공통된 차량 수준 결과인 **비의도 조향**을 주요 Hazard로 분석한다.

본 문서에서는 Alive Counter, 조향각 유효 범위, RTE Read 결과와 같은 구체적인 진단 조건과 PWM 차단, 방향 출력 비활성화 및 정상 복귀 조건 등의 구현 방법은 정의하지 않는다. 해당 내용은 후속 기능안전 요구사항, 시스템 요구사항 및 SW 요구사항·설계 단계에서 구체화한다.

본 프로젝트는 ECU 및 모터 구동 수준의 교육용 시스템을 대상으로 하므로 실제 차량의 횡방향 거동이나 Yaw를 시험하지 않았다. 차량 수준의 결과는 HARA를 위한 개념적 분석이며, 구현 및 검증 범위는 비정상 조향 정보 감지와 위험한 모터 출력 차단으로 한정한다.

본 문서의 ASIL 값은 교육 프로젝트 내부의 설계 우선순위 설정을 위한 후보 값이며, 실제 양산 차량의 공식 ISO 26262 ASIL 판정 결과를 의미하지 않는다.

## 2. S/E/C 평가 기준

| 항목                  | 레벨    | 의미                                                  |
| ------------------- | ----- | --------------------------------------------------- |
| Severity (S)        | S0–S3 | 위험 발생 시 운전자, 탑승자 및 주변 교통 참여자에게 발생할 수 있는 피해의 심각도     |
| Exposure (E)        | E0–E4 | 해당 운행 상황에 노출될 가능성 또는 빈도                             |
| Controllability (C) | C0–C3 | 위험 발생 후 운전자 또는 주변 교통 참여자가 위험을 회피하거나 차량을 통제할 수 있는 정도 |

## 3. Hazard 정의

| Hazard ID | Potential Hazard | Hazard Description                                                 |
| :-------: | ---------------- | ------------------------------------------------------------------ |
|    H-01   | 비의도 조향           | 조향 시스템이 운전자 의도보다 크거나 반대 방향의 조향 출력을 발생시켜 차량의 안전한 방향 제어를 저해할 수 있는 위험 |

## 4. HARA 요약 워크시트

| HARA ID | Function               | Malfunctioning Behavior                         | Hazard      | Operational Situation  | Hazardous Event                                                       |  S  |  E  |  C  | ASIL Candidate | Safety Goal |
| :-----: | ---------------------- | ----------------------------------------------- | ----------- | ---------------------- | --------------------------------------------------------------------- | :-: | :-: | :-: | :------------: | ----------- |
|  HE-01  | 조향 정보를 수신하여 조향 모터를 제어함 | 조향 정보가 정상적으로 수신·갱신되지 않거나 유효하지 않은 조향 입력이 제어에 사용됨 | H-01 비의도 조향 | 차량 주행 중 조향 제어가 요구되는 상황 | 운전자 의도와 다른 조향 출력으로 차량이 의도하지 않은 방향으로 움직여 차선을 이탈하거나 주변 차량·장애물과 충돌할 수 있음 |  S3 |  E4 |  C3 |        D       | SG-01       |

### 4.1 S/E/C 평가 근거

* **Severity S3**: 주행 중 비의도 조향이 발생하면 차선 이탈이나 충돌로 이어져 생명을 위협하는 상해가 발생할 수 있다.
* **Exposure E4**: 차량 주행 중 조향 기능은 지속적으로 사용되므로 해당 운행 상황에 대한 노출 빈도가 높다고 가정하였다.
* **Controllability C3**: 운전자 의도와 반대되거나 급격한 조향 출력이 발생하면 운전자가 즉시 인지하고 회피하기 어려울 수 있다고 가정하였다.
* **ASIL Candidate D**: S3/E4/C3 조합을 기준으로 교육 프로젝트의 후보 등급으로 산정하였다.

## 5. Safety Goal

| Safety Goal ID | Safety Goal                                   | ASIL Candidate | Related Hazardous Event |
| -------------- | --------------------------------------------- | :------------: | :---------------------: |
| SG-01          | 모든 차량 운행 조건에서 운전자 의도와 다른 비의도 조향이 발생하지 않아야 한다. |        D       |          HE-01          |

Safety Goal은 차량 수준에서 방지해야 할 위험한 거동을 정의한다. 통신 이상 및 입력값 이상에 대한 감지 방법과 안전 상태 전환 방법은 Safety Goal에 포함하지 않고 후속 기능안전 요구사항에서 구체화한다.

## 6. 후속 개발 단계와의 연계

SG-01을 만족하기 위해 후속 개발 단계에서는 다음 안전 기능을 기능안전 요구사항으로 구체화한다.

* 조향 데이터의 수신 상태 및 갱신 여부 감시
* 수신된 조향 입력의 유효성 확인
* 유효하지 않은 조향 데이터의 모터 제어 사용 방지
* 이상 확인 시 위험한 조향 출력을 차단하는 안전 상태로 전환
* 정상 데이터가 일정 횟수 연속 확인된 경우에만 정상 제어로 복귀

**개발 흐름**

`Hazardous Event → Safety Goal → Functional/System Requirements → System Design → SW Requirements → SW Architecture → Implementation → Verification`

관련 산출물은 다음과 같다.

* `01_Requirements.md`: Safety Goal을 기반으로 기능안전 요구사항 및 시스템 요구사항 구체화
* `02_System_Design.md`: 시스템 구조, 기능 할당 및 인터페이스 정의
* `03_SW_Requirements.md`: SW가 만족해야 할 구체적인 진단 및 안전 제어 요구사항 정의
* `04_SW_Architecture_Design.md`: SWC 구조, 인터페이스 및 안전 메커니즘 설계
* `05_SW_Detailed_Design_Unit_Construction.md`: 상세 로직 및 구현 정의
* `06_SW_Unit_Verification.md`: SW 단위 수준 검증
* `07_SW_Integration_Verification.md`: SW 통합 및 인터페이스 검증
* `08_System_Verification.md`: 시스템 수준 Fault Injection 및 안전 동작 검증
* `Traceability_Matrix.md`: HARA부터 시스템 검증까지의 양방향 추적성 관리
