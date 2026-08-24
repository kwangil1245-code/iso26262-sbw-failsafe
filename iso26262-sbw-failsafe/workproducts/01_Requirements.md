# 요구사항 명세서 (System Requirements Specification)

**Document ID**: STEER-01-SRS  
**ISO 26262 Reference**: Part 4, Cl.6 (System Requirements Specification)  
**ASPICE Reference**: SYS.2 (System Requirements Analysis)  
**Version**: 1.4  
**Date**: 2026-08-22  
**Status**: Draft  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**Subtitle**: 조향 입력 및 내부 실행 이상 진단과 안전 상태 전환

---

> 본 문서는 시스템이 수행해야 하는 기능과 안전 동작을 정의한다. SWC 배치, CAN 메시지 형식, 진단 횟수, 제어 임계값 및 PWM 계산식과 같은 구현 방법은 하위 설계 문서에서 구체화한다.

## A. 통합 기본요구사항 (공식 표준 양식)

| Req. ID | 요약 | 설명 | 중요도/긴급도 | 요청자 | 검토자 | 변경사항 | Part |
|---|---|---|---|---|---|---|---|
| Req_001 | 조향 입력 수집 | 시스템은 운전자의 조향 입력을 수집해야 한다. | 상/중 |  |  |  | P1 |
| Req_002 | 조향 정보 전달 | 시스템은 조향 입력 정보를 입력 장치에서 조향 제어 장치로 주기적으로 전달해야 한다. | 상/상 |  |  |  | P1 |
| Req_003 | 통신 이상 감지 | 시스템은 조향 입력 정보가 정상적으로 수신되거나 갱신되지 않는 상태를 감지해야 한다. | 상/상 |  |  |  | P2 |
| Req_004 | 조향 입력 유효성 검사 | 시스템은 수신한 조향 입력이 정의된 유효 조건을 만족하는지 검사해야 한다. | 상/상 |  |  |  | P2 |
| Req_005 | 내부 실행 상태 감시 | 시스템은 조향 제어와 관련된 내부 기능이 정상적으로 실행되는지 감시해야 한다. | 상/상 |  |  |  | P2 |
| Req_006 | 안전 상태 전환 | 시스템은 조향 입력 이상 또는 내부 실행 이상이 발생하면 안전 상태로 전환해야 한다. | 상/상 |  |  |  | P3 |
| Req_007 | 안전 상태 출력 제한 | 시스템은 안전 상태에서 의도하지 않은 조향 동작이 발생하지 않도록 조향 출력을 제한해야 한다. | 상/상 |  |  |  | P3 |
| Req_008 | 정상 상태 복귀 | 시스템은 정의된 정상 조건이 지속적으로 충족된 경우에만 안전 상태에서 정상 상태로 복귀해야 한다. | 상/상 |  |  |  | P3 |
| Req_009 | 조향 제어값 계산 | 시스템은 유효한 조향 입력을 기반으로 조향 방향과 출력 크기를 계산해야 한다. | 상/중 |  |  |  | P4 |
| Req_010 | 조향 하드웨어 출력 | 시스템은 계산된 조향 제어값을 조향 출력 장치로 전달해야 한다. | 상/상 |  |  |  | P4 |
| Req_011 | 진단 상태 제공 | 시스템은 현재 동작 상태와 진단된 오류 상태를 외부에서 확인할 수 있도록 제공해야 한다. | 중/중 |  |  |  | P5 |

## B. 요구사항 요약 블록

### Part 범례

| Part | 의미 |
|---|---|
| P1 | 조향 입력 수집 및 전달 |
| P2 | 입력 및 내부 실행 진단 |
| P3 | 안전 상태 전환 및 복귀 |
| P4 | 조향 제어 및 하드웨어 출력 |
| P5 | 상태 모니터링 및 진단 정보 제공 |

### 우선순위 요구사항 요약

| 우선순위 등급 | 기준(중요도/긴급도) | 개수 | 포함 Req. ID |
|---|---|---:|---|
| Critical | 상/상 | 8개 | Req_002–Req_008, Req_010 |
| High | 상/중 | 2개 | Req_001, Req_009 |
| Medium | 중/중 | 1개 | Req_011 |

### 안전 등급 요약 (HARA 기준)

| 안전 등급 | HARA ID | 관련 Req. ID |
|---|---|---|
| ASIL D (Provisional) | HC-01 | Req_002, Req_003, Req_006, Req_007 |
| ASIL D (Provisional) | HC-02 | Req_001, Req_004, Req_006, Req_007 |
| ASIL D (Provisional) | HC-03 | Req_005–Req_007 |
| ASIL D (Provisional) | HC-04 | Req_006, Req_007 |
| ASIL C (Provisional) | HC-05 | Req_008 |
| ASIL D (Provisional) | HC-06 | Req_009, Req_010 |

> 안전 등급의 상세 판정과 Safety Goal은 [`governance/00d_HARA_Worksheet.md`](governance/00d_HARA_Worksheet.md)를 기준으로 관리한다.

## C. HARA-요구사항 추적성

| HARA ID | Safety Goal ID | 관련 시스템 요구사항 | 하위 설계에서 구체화할 내용 |
|---|---|---|---|
| HC-01 | SG-01 | Req_002, Req_003, Req_006, Req_007 | CAN 송신 주기, Alive Counter 판정 기준, Timeout 검출 조건, 안전 출력값 |
| HC-02 | SG-02 | Req_001, Req_004, Req_006, Req_007 | 조향각 자료형, 유효 범위, Invalid 판정 조건, 대체 출력값 |
| HC-03 | SG-03 | Req_005–Req_007 | WdgM 감시 대상, Checkpoint, 실행 감시 조건, Fault 상태 판정 |
| HC-04 | SG-04 | Req_006, Req_007 | NORMAL/FAIL-SAFE 상태 전이와 출력 차단 순서 |
| HC-05 | SG-05 | Req_008 | 정상 판정 조건, 연속 정상 확인 횟수, 복귀 카운터 초기화 조건 |
| HC-06 | SG-06 | Req_009, Req_010 | 방향 판정 임계값, PWM 계산식, 출력 범위, IoHwAb 채널 및 핀 연결 |

## D. 하위 문서 전개 원칙

| 시스템 요구사항 | 시스템 설계 및 SW 요구사항 전개 | 구현·검증 전개 대상 |
|---|---|---|
| Req_001, Req_002 | SYS-DES-001, SYS-DES-002 → SWR-IN-001, SWR-COM-001, SWR-COM-002 | 입력·통신 단위시험, SW 통합시험, 입력 ECU 기능 확인 |
| Req_003, Req_004 | SYS-DES-003, SYS-DES-004 → SWR-DIAG-001, SWR-DIAG-002, SWR-DIAG-003 | 통신·입력 Fault 단위·통합·시스템시험 |
| Req_005 | SYS-DES-005 → SWR-WDG-001, SWR-WDG-002 | WdgM 단위·통합시험 및 실행 이상 시스템시험 |
| Req_006, Req_007, Req_008 | SYS-DES-006, SYS-DES-007, SYS-DES-008 → SWR-SAFE-001부터 SWR-SAFE-005 | FAIL-SAFE 전환·유지·복귀 단위·통합·시스템시험 |
| Req_009, Req_010 | SYS-DES-009, SYS-DES-010 → SWR-CTRL-001, SWR-CTRL-002, SWR-ACT-001, SWR-ACT-002 | 제어 계산 단위시험, SW 통합시험, PWM·방향 시스템시험 |
| Req_011 | SYS-DES-011 → SWR-MON-001, SWR-MON-002, SWR-MON-003 | 상태·Fault 관측 통합·시스템시험 |

---

CAN ID, DLC, 10 ms 송신 주기, 조향각 범위, Alive Counter 판정 횟수, 정상 복귀 횟수, WdgM 상태값, 방향 판정 임계값 및 PWM 계산식은 하위 SW 설계 정보이므로 `04_SW_Detailed_Design_Unit_Construction.md`에서 관리한다. 전체 양방향 ID 연결은 `Traceability_Matrix.md`를 기준으로 관리한다.
