# 시스템 아키텍처 설계 명세서 (System Architecture Design Specification)

**Document ID**: STEER-02-SADS  
**ISO 26262 Reference**: Part 4, Cl.7 (System Design)  
**ASPICE Reference**: SYS.3 (System Architectural Design)  
**Version**: 2.5  
**Date**: 2026-08-28  
**Status**: Draft  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**Subtitle**: 조향 입력, ECU 간 통신, 안전 판단 및 조향 출력 시스템 구조  

---

## 1. 문서 목적

본 문서는 `01_System_Requirements.md`에서 정의한 시스템 요구사항 `Req_001~Req_010`을 시스템 요소에 할당하고, 조향 입력 장치, 입력 ECU, 차량 네트워크, 출력 ECU 및 조향 출력 장치 사이의 구조와 상호작용을 정의한다.

본 문서는 시스템 수준의 아키텍처를 다루며 SWC, Runnable, RTE API, 내부 변수, Fault 판정 횟수 및 제어 알고리즘과 같은 소프트웨어 구현 상세는 정의하지 않는다.

해당 내용은 `03_SW_Requirements.md`, `04_SW_Architecture_Design.md`, `05_SW_Detailed_Design_Unit_Construction.md`에서 단계적으로 구체화한다.

---

## 2. 시스템 목적과 범위

| 항목 | 내용 |
|---|---|
| 시스템&nbsp;목적 | 조향 정보 수신 상태, 조향 입력 유효성 및 내부 실행 상태를 감시하고, 이상 발생 시 안전 상태로 전환하여 위험한 조향 출력을 방지한다. |
| 시스템&nbsp;입력 | 운전자 조향 입력, 입력 ECU로부터 수신되는 조향 정보, 출력 ECU 내부 실행 상태 |
| 시스템&nbsp;처리 | 조향 정보 전달, 조향 정보 수신 상태 감시, 입력 유효성 판단, 내부 실행 상태 감시, 안전 상태 판단, 정상 상태 복귀, 조향 출력 계산 및 제한 |
| 시스템&nbsp;출력 | 조향 방향 및 출력 크기 |
| 시스템&nbsp;경계 | 조향 입력 장치부터 입력 ECU, CAN Network, 출력 ECU 및 조향 출력 장치까지 포함한다. |
| 범위&nbsp;외 | 실제 차량 조향 동역학, 운전자 경고 HMI, 외부 상태 표시 기능, 양산 수준의 이중화 및 Fail-Operational 제어 |

---

## 3. 시스템 컨텍스트

```mermaid
flowchart LR
    A["운전자"] --> B["조향 입력 장치"]
    B --> C["조향 오류 복구·진단 시스템"]
    C --> D["조향 출력 장치"]
