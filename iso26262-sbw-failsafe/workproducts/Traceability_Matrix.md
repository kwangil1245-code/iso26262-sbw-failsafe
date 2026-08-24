# 통합 추적성 매트릭스 (Integrated Traceability Matrix)

**Document ID**: STEER-TM-001  
**Version**: 2.1  
**Date**: 2026-08-24  
**Status**: Baseline  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템

---

## 1. 목적과 적용 기준

본 문서는 시스템 요구사항부터 시스템검증까지의 양방향 추적성을 관리하는 기준 문서이다. 공식 ID 체계는 다음과 같다.

```text
HARA/SG → Req → SYS-F/SYS-DES → SWR → SWC/SW-IF/RUN
→ SWD/UNIT → UT → ITC → SYS-TC
```

- `archive_SW_Qualification_Verification.md`의 `SVT-*`는 Superseded 상태이므로 공식 추적성에서 제외한다.
- SWE.5는 SWC Interface와 Runnable 통합만 검증한다.
- 최종 외부 동작은 `08_System_Verification.md`의 입력 ECU 기능 확인 및 CAPL Restbus 시스템시험으로 검증한다.
- `SYS-TC-IN-*`은 실제 입력 ECU 기능 확인, 나머지 `SYS-TC-*`은 CAPL이 입력 ECU를 대체하는 출력 시스템시험이다.

## 2. 시스템–SW–검증 통합 추적성

| Req ID | SYS 설계 | SW Req ID | SW 아키텍처 | 상세설계·Unit | 단위시험 | SW 통합시험 | 시스템시험 | 상태 |
|---|---|---|---|---|---|---|---|---|
| Req_001 | SYS-F-001 / SYS-DES-001 | SWR-IN-001 | SWC-001 / SW-IF-001 / RUN-001 | SWD-IN-001, SWD-IN-002 / UNIT-001 | UT-IN-001부터 UT-IN-005 | ITC-SW-001 | SYS-TC-IN-001 | Verified |
| Req_002 | SYS-F-002 / SYS-DES-002 | SWR-COM-001 | SWC-001 / SW-IF-002 / RUN-001 | SWD-COM-001, SWD-COM-002 / UNIT-001 | UT-IN-001, UT-IN-004, UT-IN-005 | ITC-SW-002, ITC-SW-003, ITC-FLOW-001 | SYS-TC-IN-002, SYS-TC-IN-003 | Verified |
| Req_002 | SYS-F-002 / SYS-DES-002 | SWR-COM-002 | SWC-002 / SW-IF-002 / RUN-002 | SWD-DIAG-001 / UNIT-002 | UT-DIAG-001, UT-DIAG-002 | ITC-SW-002, ITC-FLOW-001 | SYS-TC-IN-002, SYS-TC-IN-003 | Verified |
| Req_003 | SYS-F-003 / SYS-DES-003 | SWR-DIAG-001 | SWC-002 / SW-IF-003 / RUN-002 | SWD-DIAG-001, SWD-DIAG-003, SWD-DIAG-004, SWD-DIAG-005, SWD-DIAG-006 / UNIT-002 | UT-DIAG-001, UT-DIAG-002, UT-DIAG-007부터 UT-DIAG-010 | ITC-SW-003, ITC-SW-005 | SYS-TC-FLT-001 | Verified |
| Req_004 | SYS-F-004 / SYS-DES-004 | SWR-DIAG-002 | SWC-002 / SW-IF-003 / RUN-002 | SWD-DIAG-002 / UNIT-002 | UT-DIAG-003부터 UT-DIAG-006 | ITC-SW-004, ITC-SW-006 | SYS-TC-FLT-002, SYS-TC-FLT-003 | Verified |
| Req_003, Req_004 | SYS-F-003, SYS-F-004 / SYS-DES-003, SYS-DES-004 | SWR-DIAG-003 | SWC-002 / SW-IF-003 / RUN-002 | SWD-DIAG-007 / UNIT-002 | UT-DIAG-001, UT-DIAG-002, UT-DIAG-003, UT-DIAG-006 | ITC-SW-004, ITC-SW-005, ITC-SW-006, ITC-FLOW-002 | SYS-TC-FLT-001, SYS-TC-FLT-002, SYS-TC-FLT-003 | Verified |
| Req_005 | SYS-F-005 / SYS-DES-005 | SWR-WDG-001 | SWC-003 / SW-IF-004 / RUN-003 | SWD-SAFE-001, SWD-WDG-001 / UNIT-003, UNIT-004 | UT-SAFE-001, UT-WDG-001부터 UT-WDG-005 | ITC-SW-007, ITC-SW-008 | SYS-TC-FLT-005 | Verified |
| Req_005 | SYS-F-005 / SYS-DES-005 | SWR-WDG-002 | SWC-003 / SW-IF-004 / RUN-003 | SWD-SAFE-002, SWD-WDG-001, SWD-WDG-002 / UNIT-003, UNIT-004 | UT-SAFE-003, UT-WDG-001부터 UT-WDG-005 | ITC-SW-007, ITC-SW-008, ITC-SW-009 | SYS-TC-FLT-005 | Verified |
| Req_006 | SYS-F-006 / SYS-DES-006 | SWR-SAFE-001 | SWC-003 / SW-IF-003, SW-IF-004, SW-IF-005 / RUN-003 | SWD-SAFE-002, SWD-SAFE-003 / UNIT-003 | UT-SAFE-002, UT-SAFE-003 | ITC-SW-010, ITC-SW-011, ITC-FLOW-003, ITC-FLOW-005 | SYS-TC-FLT-001부터 SYS-TC-FLT-005 | Verified |
| Req_007 | SYS-F-007 / SYS-DES-007 | SWR-SAFE-002 | SWC-003, SWC-004, SWC-005 / SW-IF-005, SW-IF-006, SW-IF-007 / RUN-003, RUN-004, RUN-005 | SWD-SAFE-004, SWD-CTRL-001, SWD-ACT-002 / UNIT-003, UNIT-005, UNIT-006 | UT-SAFE-002, UT-SAFE-003, UT-CTRL-001, UT-ACT-001 | ITC-SW-011, ITC-SW-015, ITC-FLOW-005 | SYS-TC-FLT-001부터 SYS-TC-FLT-005 | Verified |
| Req_006, Req_007 | SYS-F-006, SYS-F-007 / SYS-DES-006, SYS-DES-007 | SWR-SAFE-003 | SWC-003, SWC-004, SWC-005 / SW-IF-005, SW-IF-006, SW-IF-007 / RUN-003, RUN-004, RUN-005 | SWD-SAFE-003, SWD-SAFE-004, SWD-CTRL-001, SWD-ACT-002 / UNIT-003, UNIT-005, UNIT-006 | UT-SAFE-008, UT-CTRL-001, UT-ACT-001 | ITC-SW-011, ITC-SW-015, ITC-FLOW-005 | SYS-TC-FLT-004 | Verified |
| Req_008 | SYS-F-008 / SYS-DES-008 | SWR-SAFE-004 | SWC-003 / SW-IF-005 / RUN-003 | SWD-SAFE-005, SWD-SAFE-006 / UNIT-003 | UT-SAFE-004, UT-SAFE-005, UT-SAFE-006 | SWE.5 비대상; 단위 상태 로직 검증 | SYS-TC-REC-001, SYS-TC-REC-002 | Verified |
| Req_008 | SYS-F-008 / SYS-DES-008 | SWR-SAFE-005 | SWC-003 / SW-IF-005 / RUN-003 | SWD-SAFE-007 / UNIT-003 | UT-SAFE-007 | SWE.5 비대상; 단위 상태 로직 검증 | SYS-TC-REC-003 | Verified |
| Req_009 | SYS-F-009 / SYS-DES-009 | SWR-CTRL-001 | SWC-004 / SW-IF-005, SW-IF-006 / RUN-004 | SWD-CTRL-002, SWD-CTRL-003, SWD-CTRL-005, SWD-CTRL-006, SWD-CTRL-007, SWD-CTRL-009 / UNIT-005 | UT-CTRL-003, UT-CTRL-005부터 UT-CTRL-009 | ITC-SW-010, ITC-SW-012, ITC-FLOW-003, ITC-FLOW-004 | SYS-TC-OUT-001, SYS-TC-OUT-002 | Verified |
| Req_009 | SYS-F-009 / SYS-DES-009 | SWR-CTRL-002 | SWC-004 / SW-IF-005, SW-IF-006 / RUN-004 | SWD-CTRL-004, SWD-CTRL-008 / UNIT-005 | UT-CTRL-002, UT-CTRL-004 | ITC-SW-013, ITC-FLOW-004 | SYS-TC-OUT-003 | Verified |
| Req_010 | SYS-F-010 / SYS-DES-010 | SWR-ACT-001 | SWC-005 / SW-IF-006, SW-IF-007 / RUN-005 | SWD-ACT-001, SWD-ACT-003 / UNIT-006 | UT-ACT-002, UT-ACT-003, UT-ACT-004 | ITC-SW-012, ITC-SW-014, ITC-FLOW-004 | SYS-TC-OUT-001, SYS-TC-OUT-002 | Verified |
| Req_007, Req_010 | SYS-F-007, SYS-F-010 / SYS-DES-007, SYS-DES-010 | SWR-ACT-002 | SWC-005 / SW-IF-006, SW-IF-007 / RUN-005 | SWD-ACT-002 / UNIT-006 | UT-ACT-001 | ITC-SW-013, ITC-SW-015 | SYS-TC-OUT-003, SYS-TC-FLT-001부터 SYS-TC-FLT-005 | Verified |
| Req_011 | SYS-F-011 / SYS-DES-011 | SWR-MON-001 | SWC-003 / SW-IF-008 / RUN-003 | SWD-SAFE-008 / UNIT-003 | UT-SAFE-001, UT-SAFE-002, UT-SAFE-003 | ITC-SW-016 | SYS-TC-MON-001 | Verified |
| Req_011 | SYS-F-011 / SYS-DES-011 | SWR-MON-002 | SWC-003 / SW-IF-008 / RUN-003 | SWD-SAFE-008, SWD-WDG-002 / UNIT-003, UNIT-004 | UT-WDG-003, UT-SAFE-002, UT-SAFE-003 | ITC-SW-009, ITC-SW-016 | SYS-TC-MON-001 | Verified |
| Req_011 | SYS-F-011 / SYS-DES-011 | SWR-MON-003 | SWC-003, SWC-004, SWC-005 / SW-IF-005, SW-IF-006, SW-IF-007, SW-IF-008 / RUN-003, RUN-004, RUN-005 | SWD-SAFE-008, SWD-ACT-004 / UNIT-003, UNIT-005, UNIT-006 | UT-ACT-001, UT-ACT-005 | ITC-SW-016 | SYS-TC-MON-001 | Verified |

## 3. HARA–Safety Goal–시스템검증 추적성

| HARA ID | Safety Goal | 시스템 요구사항 | 시스템 Test Case | 상태 |
|---|---|---|---|---|
| HC-01 | SG-01 | Req_002, Req_003, Req_006, Req_007 | SYS-TC-IN-002, SYS-TC-IN-003, SYS-TC-FLT-001, SYS-TC-FLT-004 | Verified |
| HC-02 | SG-02 | Req_001, Req_004, Req_006, Req_007 | SYS-TC-IN-001, SYS-TC-FLT-002, SYS-TC-FLT-003, SYS-TC-FLT-004 | Verified |
| HC-03 | SG-03 | Req_005, Req_006, Req_007 | SYS-TC-FLT-005 | Verified |
| HC-04 | SG-04 | Req_006, Req_007 | SYS-TC-FLT-001, SYS-TC-FLT-002, SYS-TC-FLT-003, SYS-TC-FLT-005 | Verified |
| HC-05 | SG-05 | Req_008 | SYS-TC-REC-001, SYS-TC-REC-002, SYS-TC-REC-003 | Verified |
| HC-06 | SG-06 | Req_009, Req_010 | SYS-TC-OUT-001, SYS-TC-OUT-002, SYS-TC-OUT-003 | Verified |

## 4. 산출물 기준선

| 단계 | 기준 문서 | 주요 ID |
|---|---|---|
| 시스템 요구사항 | `01_Requirements.md` | Req_001부터 Req_011 |
| 시스템 설계 | `02_System_Design.md` | SYS-F-001부터 SYS-F-011, SYS-DES-001부터 SYS-DES-011 |
| SW 요구사항 | `03_SW_Requirements.md` | SWR-IN, SWR-COM, SWR-DIAG, SWR-WDG, SWR-SAFE, SWR-CTRL, SWR-ACT, SWR-MON |
| SW 아키텍처 | `04_SW_Architecture_Design.md` | SWC-001부터 SWC-005, SW-IF-001부터 SW-IF-008, RUN-001부터 RUN-005 |
| SW 상세설계·구현 | `05_SW_Detailed_Design_Unit_Construction.md` | SWD-*, UNIT-001부터 UNIT-006 |
| SW 단위검증 | `06_SW_Unit_Verification.md` | UT-*, SA-* |
| SW 통합검증 | `07_SW_Integration_Verification.md` | ITC-SW-*, ITC-FLOW-* |
| 시스템 요구사항 검증 | `08_System_Verification.md` | SYS-TC-IN-*, SYS-TC-OUT-*, SYS-TC-FLT-*, SYS-TC-REC-*, SYS-TC-MON-* |

## 5. 변경 영향 관리

- `Req_*` 변경 시 관련 `SYS-F`, `SYS-DES`, `SWR`, `SWD`, `UT`, `ITC`, `SYS-TC`를 함께 검토한다.
- Interface 변경 시 `SW-IF-*`, 연결 SWC, RTE API, `ITC-SW-*` 및 관련 시스템시험을 재검토한다.
- 진단 임계값이나 Counter 변경 시 상세설계, 경계값 단위시험, CAPL Fault Injection 조건 및 정상 복귀 시험을 함께 변경한다.
- HW Mapping 변경 시 UNIT-006, IoHwAb 연동, SW 통합 Mock 및 PWM·방향 시스템시험을 재수행한다.
- 모든 변경은 Commit ID, 변경 사유, 영향받는 ID 및 재시험 결과와 연결한다.

---

본 문서는 프로젝트 산출물 간 추적성의 최종 SSOT이다. 공식 추적성 판정은 본 문서의 현재 ID와 `Verified` 상태를 기준으로 한다.
