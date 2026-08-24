# 시스템 요구사항 검증 명세서·결과서

**Document ID**: STEER-08-SYSQT  
**ISO 26262 Reference**: Part 4, Cl.8  
**ASPICE Reference**: SYS.5 (System Qualification Test)  
**Version**: 1.2  
**Date**: 2026-08-24  
**Status**: Completed  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템

---

## 1. 문서 목적

본 문서는 `01_Requirements.md`의 시스템 요구사항 `Req_001`부터 `Req_011`이 통합 시스템에서 충족되는지 검증한 결과를 기록한다.

주 시스템시험에서는 CANoe/CAPL Restbus Simulation이 조향 입력 장치와 입력 ECU를 대체한다. CAPL은 정상 조향값, Alive Counter, Timeout 및 Invalid 조건을 생성하고, 시험 대상인 실제 출력 ECU와 조향 출력 장치의 PWM·방향·정지 동작을 확인한다.

CAPL이 대체하는 입력 ECU의 자체 기능은 주 시스템시험에서 직접 검증할 수 없으므로, `Req_001`과 입력 ECU 송신 책임에 해당하는 `Req_002`는 별도의 입력 ECU 기능 확인 결과로 연결한다.

## 2. 검증 구성

### 구성 A: 입력 ECU 기능 확인

```mermaid
flowchart LR
    P["가변저항"] --> I["실제 입력 ECU"]
    I --> T["CANoe Trace"]
```

| 대상 | 확인 내용 |
|---|---|
| 조향 입력 장치 | 최소·중간·최대 조향 입력 제공 |
| 입력 ECU | 조향값 변환, Alive Counter 생성 및 주기 송신 |
| CANoe | CAN Signal, Alive Counter 및 송신 주기 관측 |

### 구성 B: CAPL Restbus 기반 출력 시스템 검증

```mermaid
flowchart LR
    C["CANoe·CAPL 가상 입력 ECU"] --> N["CAN Network"]
    N --> O["실제 출력 ECU"]
    O --> A["PWM·방향 Pin·LED·모터"]
```

| 구분 | 구성 |
|---|---|
| 환경 모델 | CANoe/CAPL 가상 입력 ECU |
| 시험 대상 | 실제 출력 ECU, IoHwAb 출력 및 조향 출력 장치 |
| 정상 자극 | 유효 조향값과 증가하는 Alive Counter를 10 ms 주기로 송신 |
| Fault 자극 | 동일 Counter 반복, 유효 범위 밖 조향값, WdgM 시험 설정 |
| 관측 | CAN Trace, PWM, 방향 Pin, 상태 LED 및 모터 |

## 3. 단계별 산출물 관계

| 단계 | 프로젝트 적용 |
|---|---|
| SWE.4 | `06_SW_Unit_Verification.md`: Stub/Mock 기반 단위검증 |
| SWE.5 | `07_SW_Integration_Verification.md`: PC 기반 SWC Interface와 Runnable 통합검증 |
| SWE.6 | 별도 공식 산출물로 사용하지 않음 |
| SYS.5 | 본 문서: 입력 ECU 기능 확인 + CAPL Restbus 기반 출력 시스템 요구사항 검증 |

> `archive_SW_Qualification_Verification.md`는 폐기된 초안이며 공식 추적성 기준에서 제외한다.

## 4. 시험 환경

| 항목 | 구성 |
|---|---|
| 입력 ECU 기능 확인 | MPC-5606B 입력 ECU, 가변저항, CANoe |
| 출력 시스템 검증 | CANoe/CAPL, MPC-5606B 출력 ECU, PWM·방향·LED·모터 |
| AUTOSAR 환경 | Mobilgene Classic 기반 생성 코드 및 SWC |
| 빌드·다운로드 | CodeWarrior |
| CAN 관측 | CANoe Trace 및 Signal |
| PWM 관측 | PWM 계측값 또는 오실로스코프·로직 분석기 |
| 내부 실행 이상 | WdgM Checkpoint 누락 또는 시험 설정을 통한 실행 이상 생성 |
| 보조 분석 | 필요 시 CodeWarrior를 이용한 원인 확인; 최종 판정은 외부 출력 기준 |

## 5. 시험 전제조건

- 시험 대상 ECU에 식별 가능한 Software Build가 다운로드되어야 한다.
- CAN DB와 CAPL Signal Mapping이 ECU 설정과 일치해야 한다.
- 출력 ECU의 PWM, 방향 Pin 및 상태 LED가 지정된 IoHwAb 채널에 연결되어야 한다.
- 각 Test Case 시작 전에 Fault와 정상 복귀 상태를 초기화해야 한다.
- CAPL에서 정상 송신, Counter 고정, Invalid 값 및 정상 복귀 시나리오를 선택할 수 있어야 한다.

## 6. 시스템 Test Case

### 6.1 구성 A — 입력 ECU 기능 확인

| SYS-TC ID | 시험 조건·절차 | 기대 결과 | 관측 방법 | 추적 ID | 결과 |
|---|---|---|---|---|---|
| SYS-TC-IN-001 | 가변저항을 최소·중간·최대 위치로 변경 | 입력 변화에 대응하는 조향값이 생성됨 | CANoe Signal/Trace | Req_001 / SYS-F-001 / SYS-DES-001 | PASS |
| SYS-TC-IN-002 | 입력 ECU 정상 동작 중 조향 메시지 관측 | 조향값과 Alive Counter가 10 ms 주기로 송신됨 | CANoe Trace | Req_002 / SYS-F-002 / SYS-DES-002 | PASS |
| SYS-TC-IN-003 | 조향 메시지를 연속 관측 | Alive Counter가 메시지마다 증가하고 조향값이 정상 범위로 전달됨 | CANoe Trace | Req_002 / SYS-F-002 / SYS-DES-002 | PASS |

### 6.2 구성 B — 정상 조향 출력

| SYS-TC ID | CAPL 시험 조건 | 기대 결과 | 관측 방법 | 추적 ID | 결과 |
|---|---|---|---|---|---|
| SYS-TC-OUT-001 | 정상 Counter와 증가하는 조향값 송신 | 우측 방향과 입력 변화에 대응하는 PWM 출력 발생 | PWM, 방향 Pin, 모터 | Req_009, Req_010 / SYS-F-009, SYS-F-010 / SYS-DES-009, SYS-DES-010 | PASS |
| SYS-TC-OUT-002 | 정상 Counter와 감소하는 조향값 송신 | 좌측 방향과 입력 변화에 대응하는 PWM 출력 발생 | PWM, 방향 Pin, 모터 | Req_009, Req_010 / SYS-F-009, SYS-F-010 / SYS-DES-009, SYS-DES-010 | PASS |
| SYS-TC-OUT-003 | 조향 변화량을 정지 조건 이내로 송신 | PWM과 좌·우 방향이 비활성화되고 모터 정지 | PWM, 방향 Pin, 모터 | Req_009, Req_010 / SYS-F-009, SYS-F-010 / SYS-DES-009, SYS-DES-010 | PASS |

### 6.3 구성 B — 통신·입력 Fault 및 FAIL-SAFE

| SYS-TC ID | CAPL 시험 조건 | 기대 결과 | 관측 방법 | 추적 ID | 결과 |
|---|---|---|---|---|---|
| SYS-TC-FLT-001 | Alive Counter를 동일 값으로 반복 송신 | 갱신 이상 감지, FAIL-SAFE 전환, PWM·방향 출력 차단 | CANoe Trace, PWM, 방향 Pin, LED, 모터 | Req_003, Req_006, Req_007 / SYS-F-003, SYS-F-006, SYS-F-007 / SYS-DES-003, SYS-DES-006, SYS-DES-007 | PASS |
| SYS-TC-FLT-002 | 정상 범위 미만의 조향값 송신 | Invalid 감지, FAIL-SAFE 전환, 출력 차단 | CANoe Trace, PWM, 방향 Pin, LED, 모터 | Req_004, Req_006, Req_007 / SYS-F-004, SYS-F-006, SYS-F-007 / SYS-DES-004, SYS-DES-006, SYS-DES-007 | PASS |
| SYS-TC-FLT-003 | 정상 범위 초과의 조향값 송신 | Invalid 감지, FAIL-SAFE 전환, 출력 차단 | CANoe Trace, PWM, 방향 Pin, LED, 모터 | Req_004, Req_006, Req_007 / SYS-F-004, SYS-F-006, SYS-F-007 / SYS-DES-004, SYS-DES-006, SYS-DES-007 | PASS |
| SYS-TC-FLT-004 | Fault 조건을 유지하며 조향값 변경 | FAIL-SAFE와 PWM·방향 출력 차단 유지 | PWM, 방향 Pin, LED, 모터 | Req_006, Req_007 / SYS-F-006, SYS-F-007 / SYS-DES-006, SYS-DES-007 | PASS |
| SYS-TC-FLT-005 | WdgM Checkpoint 누락 또는 실행 이상 시험 설정 | 내부 실행 이상 감지, FAIL-SAFE 전환, 출력 차단 | PWM, 방향 Pin, LED, 모터; CodeWarrior 보조 확인 | Req_005, Req_006, Req_007 / SYS-F-005, SYS-F-006, SYS-F-007 / SYS-DES-005, SYS-DES-006, SYS-DES-007 | PASS |

### 6.4 구성 B — 정상 상태 복귀

| SYS-TC ID | CAPL 시험 조건 | 기대 결과 | 관측 방법 | 추적 ID | 결과 |
|---|---|---|---|---|---|
| SYS-TC-REC-001 | Fault 해제 후 정상 메시지 1회와 2회 송신 | 복귀 기준 충족 전까지 FAIL-SAFE와 출력 차단 유지 | CANoe Trace, PWM, 방향 Pin, LED | Req_008 / SYS-F-008 / SYS-DES-008 | PASS |
| SYS-TC-REC-002 | Fault 해제 후 정상 메시지 연속 3회 송신 | 3회째 NORMAL 복귀 후 조향 출력 재활성화 | CANoe Trace, PWM, 방향 Pin, LED, 모터 | Req_008 / SYS-F-008 / SYS-DES-008 | PASS |
| SYS-TC-REC-003 | 정상 메시지 2회 후 Timeout 또는 Invalid 재주입 | 정상 복귀 중단, FAIL-SAFE와 출력 차단 유지 | CANoe Trace, PWM, 방향 Pin, LED | Req_008 / SYS-F-008 / SYS-DES-008 | PASS |

### 6.5 구성 B — 상태 및 Fault 관측

| SYS-TC ID | 시험 조건 | 기대 결과 | 관측 방법 | 추적 ID | 결과 |
|---|---|---|---|---|---|
| SYS-TC-MON-001 | NORMAL, Timeout, Invalid, WdgM Fault 및 복귀 조건 순차 실행 | 상태와 Fault가 CAN 정보, LED 또는 외부 출력 결과로 구분됨 | CANoe, LED, PWM, 방향 Pin | Req_011 / SYS-F-011 / SYS-DES-011 | PASS |

## 7. 시스템 요구사항 추적성

| 시스템 요구사항 | 검증 Test Case | 검증 구성 |
|---|---|---|
| Req_001 | SYS-TC-IN-001 | 입력 ECU 기능 확인 |
| Req_002 | SYS-TC-IN-002, SYS-TC-IN-003 | 입력 ECU 기능 확인 |
| Req_003 | SYS-TC-FLT-001 | CAPL Restbus 출력 시스템 |
| Req_004 | SYS-TC-FLT-002, SYS-TC-FLT-003 | CAPL Restbus 출력 시스템 |
| Req_005 | SYS-TC-FLT-005 | WdgM 실행 이상 시험 설정 |
| Req_006 | SYS-TC-FLT-001, SYS-TC-FLT-002, SYS-TC-FLT-003, SYS-TC-FLT-004, SYS-TC-FLT-005 | CAPL Restbus 출력 시스템 |
| Req_007 | SYS-TC-FLT-001, SYS-TC-FLT-002, SYS-TC-FLT-003, SYS-TC-FLT-004, SYS-TC-FLT-005 | CAPL Restbus 출력 시스템 |
| Req_008 | SYS-TC-REC-001, SYS-TC-REC-002, SYS-TC-REC-003 | CAPL Restbus 출력 시스템 |
| Req_009 | SYS-TC-OUT-001, SYS-TC-OUT-002, SYS-TC-OUT-003 | CAPL Restbus 출력 시스템 |
| Req_010 | SYS-TC-OUT-001, SYS-TC-OUT-002, SYS-TC-OUT-003 | CAPL Restbus 출력 시스템 |
| Req_011 | SYS-TC-MON-001 | CAPL Restbus 출력 시스템 |

## 8. HARA 및 Safety Goal 연결

| HARA ID | Safety Goal | 관련 시스템 Test Case |
|---|---|---|
| HC-01 | SG-01 | SYS-TC-FLT-001, SYS-TC-FLT-004 |
| HC-02 | SG-02 | SYS-TC-FLT-002, SYS-TC-FLT-003, SYS-TC-FLT-004 |
| HC-03 | SG-03 | SYS-TC-FLT-005 |
| HC-04 | SG-04 | SYS-TC-FLT-001, SYS-TC-FLT-002, SYS-TC-FLT-003, SYS-TC-FLT-005 |
| HC-05 | SG-05 | SYS-TC-REC-001, SYS-TC-REC-002, SYS-TC-REC-003 |
| HC-06 | SG-06 | SYS-TC-OUT-001, SYS-TC-OUT-002, SYS-TC-OUT-003 |

## 9. 시험 결과 요약

| 시험 구성·그룹 | 전체 | PASS | FAIL | BLOCKED |
|---|---:|---:|---:|---:|
| 입력 ECU 기능 확인 | 3 | 3 | 0 | 0 |
| 정상 조향 출력 | 3 | 3 | 0 | 0 |
| 통신·입력·실행 Fault 및 FAIL-SAFE | 5 | 5 | 0 | 0 |
| 정상 상태 복귀 | 3 | 3 | 0 | 0 |
| 상태 및 Fault 관측 | 1 | 1 | 0 | 0 |
| 합계 | 15 | 15 | 0 | 0 |

### 수행 결과

- 입력 ECU의 조향값·Alive Counter 생성과 주기 송신을 별도 구성에서 확인하였다.
- CAPL Restbus를 이용하여 실제 입력 ECU 없이 정상·Timeout·Invalid 조건을 반복 생성하였다.
- 실제 출력 ECU에서 PWM·방향·LED·모터의 정상 동작과 FAIL-SAFE 출력 차단을 확인하였다.
- Fault 지속 중 안전 출력 유지와 정상 조건 충족 후 출력 재활성화를 확인하였다.
- `Req_001`부터 `Req_011`과 Safety Goal 6개가 시스템 Test Case에 연결되었다.

> CAPL Source, CANoe Configuration, CAN Trace, PWM 계측 결과, Pin·LED·모터 사진 또는 영상은 각 `SYS-TC-*` ID와 연결하여 증적으로 관리한다.

## 10. 합격 기준 및 회귀시험

- 모든 `Req_*`가 하나 이상의 `SYS-TC-*`에 연결되어야 한다.
- 모든 Test Case가 PASS이거나 승인된 편차와 연결되어야 한다.
- Software Build, CAN DB, CAPL 및 CANoe Configuration 버전이 시험 결과와 연결되어야 한다.
- 요구사항·CAN Signal·진단 조건·상태 전이·HW Mapping 변경 시 영향받는 Test Case를 재수행해야 한다.
- FAIL 발생 시 결함 ID, 수정 Commit 및 재시험 결과를 연결해야 한다.

---

본 문서는 입력 ECU 기능 확인과 CANoe/CAPL Restbus 기반 출력 시스템 시험을 결합한 SYS.5 시스템 요구사항 검증의 기준 산출물이다.
