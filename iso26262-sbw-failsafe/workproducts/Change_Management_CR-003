# 변경관리 시나리오: FAIL-SAFE 정상 복귀 안정화 및 Fault 원인 구분 개선

**Change Request ID**: CR-003  
**Title**: FAIL-SAFE 정상 복귀 안정화 및 Fault 진단성 개선  
**Version**: 0.2  
**Date**: 2026-08-24  
**Status**: Planned  
**Project**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템

---

## 1. 변경 배경과 목적

초기 설계에서는 Fault가 해제되고 정상 메시지가 1회 수신되면 FAIL-SAFE에서 NORMAL로 즉시 복귀하였다. CAPL 기반 시스템검증에서 정상 메시지와 비정상 메시지가 번갈아 수신되는 간헐적 Fault 조건을 적용한 결과, 시스템 상태가 NORMAL과 FAIL-SAFE 사이에서 반복 전환되고 PWM 출력이 순간적으로 재활성화될 수 있는 문제를 확인하였다.

또한 산출물과 코드의 추적성을 검토한 결과, `SWR-MON-002`는 통신·조향 입력·내부 실행 Fault를 구분하여 제공하도록 요구하지만 실제 구현은 모든 Fault를 하나의 Boolean Flag로 통합하고 있었다. 이 때문에 FAIL-SAFE 진입 여부는 확인할 수 있으나 원인을 외부에서 식별할 수 없었다.

이에 다음 두 항목을 변경한다.

1. 정상 복귀 조건을 정상 1회 확인에서 정상 조건 3회 연속 확인으로 강화한다.
2. 단일 Boolean Fault Flag를 원인별 비트맵 `FaultCode`로 변경한다.

## 2. 변경요청

### 2.1 정상 복귀 조건 강화

| 항목 | 내용 |
|---|---|
| 발견 단계 | 시스템검증 |
| 관련 시험 | SYS-TC-REC-001 |
| 변경 전 | 정상 메시지 1회 수신 후 NORMAL 복귀 |
| 변경 후 | 정상 조건 3회 연속 확인 후 NORMAL 복귀 |
| 추가 조건 | 복귀 확인 중 Fault 재발 시 Counter 초기화 및 FAIL-SAFE 유지 |
| 변경 사유 | 간헐적 Fault에서 상태 반복 전환과 출력 재활성화 방지 |
| 변경 유형 | 안전 상태 전이 요구사항 변경 |

### 2.2 Fault 원인 구분

| 항목 | 내용 |
|---|---|
| 발견 단계 | 산출물–코드 추적성 검토 및 시스템 모니터링 검증 |
| 관련 시험 | SYS-TC-MON-001 |
| 변경 전 | 통신·입력·WdgM Fault를 단일 Boolean Flag로 전달 |
| 변경 후 | Fault 원인을 비트맵 `FaultCode`로 구분하여 전달·관측 |
| 변경 사유 | FAIL-SAFE 진입 원인 식별 및 `SWR-MON-002`와 구현의 일치 확보 |
| 안전 영향 | Fault 원인 미구분 시 고장 분석과 조치 판단이 지연될 수 있음 |
| 변경 유형 | 진단 인터페이스 및 모니터링 요구사항 변경 |

## 3. 변경 전 문제 재현

### 3.1 정상 복귀 문제

| 수신 순서 | 변경 전 상태 | 문제점 |
|---|---|---|
| Fault | FAIL-SAFE | 정상 안전 전환 |
| 정상 1회 | NORMAL | 너무 빠른 정상 복귀 |
| Fault 재발 | FAIL-SAFE | 상태 재전환 |
| 정상 1회 | NORMAL | PWM 출력 재활성화 가능 |

### 3.2 Fault 원인 미구분 문제

```c
boolean flag = FALSE;

if (rteReadError == TRUE)
{
    flag = TRUE;
}
else if (aliveCounterError == TRUE)
{
    flag = TRUE;
}
else if (steerInvalid == TRUE)
{
    flag = TRUE;
}
```

모든 고장이 `TRUE`로만 전달되므로 SafetyPolicy와 CANoe 모니터링 환경에서 실제 원인을 구분할 수 없다. 복수 Fault가 동시에 발생한 경우에도 하나의 상태로만 표현된다.

## 4. 요구사항 변경

### 4.1 Req_008 변경

**변경 전**

> Fault가 해제되면 시스템은 FAIL-SAFE 상태에서 NORMAL 상태로 복귀해야 한다.

**변경 후**

> 시스템은 FAIL-SAFE 상태에서 Fault가 없는 정상 조건이 3회 연속 확인된 경우에만 NORMAL 상태로 복귀해야 한다. 정상 확인 중 Fault가 다시 발생하면 정상 확인 횟수를 초기화하고 FAIL-SAFE 상태를 유지해야 한다.

### 4.2 Req_011 변경

**변경 전**

> 시스템은 현재 상태와 Fault 정보를 외부 진단·모니터링 환경에 제공해야 한다.

**변경 후**

> 시스템은 현재 상태와 Fault 정보를 외부 진단·모니터링 환경에 제공해야 하며, 통신 갱신 이상, 조향 입력 Invalid 및 내부 실행 이상을 원인별로 구분할 수 있어야 한다. 복수 Fault가 동시에 발생하면 각 원인을 함께 식별할 수 있어야 한다.

### 4.3 하위 SW 요구사항

> **SWR-SAFE-004**: SW는 FAIL-SAFE 상태에서 정상 조건이 확인될 때마다 정상 복귀 Counter를 증가시키고, 3회 연속 확인된 경우 NORMAL 상태로 전환해야 한다.

> **SWR-SAFE-005**: SW는 정상 복귀 조건 확인 중 Fault가 다시 발생하면 정상 복귀 Counter를 0으로 초기화해야 한다.

> **SWR-MON-002**: SW는 RTE Read 실패, Alive Counter 갱신 이상, 조향값 Invalid 및 WdgM 실행 이상을 원인별 FaultCode로 제공해야 하며, 복수 Fault를 동시에 표현할 수 있어야 한다.

## 5. 추적성 및 영향 분석

### 5.1 정상 복귀 조건

```text
CR-003
└─ HC-05 / SG-05
   └─ Req_008
      └─ SYS-F-008 / SYS-DES-008
         └─ SWR-SAFE-004 / SWR-SAFE-005
            └─ SWC-003 / SW-IF-005 / RUN-003
               └─ SWD-SAFE-005 / SWD-SAFE-006 / SWD-SAFE-007
                  ├─ UT-SAFE-004–007
                  └─ SYS-TC-REC-001–003
```

### 5.2 Fault 원인 구분

```text
CR-003
└─ Req_011
   └─ SYS-F-011 / SYS-DES-011
      └─ SWR-MON-001 / SWR-MON-002 / SWR-MON-003
         └─ SWC-002 / SWC-003 / SW-IF-003 / SW-IF-005 / SW-IF-008
            └─ SWD-DIAG-* / SWD-WDG-* / SWD-SAFE-002 / SWD-SAFE-008
               ├─ App_CanMonitor.c / App_SafetyPolicy.c
               ├─ UT-DIAG-* / UT-WDG-* / UT-SAFE-*
               ├─ ITC-SW-009 / ITC-SW-016
               └─ SYS-TC-MON-001
```

| 대상 | 영향 구분 | 조치 |
|---|---|---|
| HC-05 / SG-05 | 직접·간접 영향 | 복귀 불안정 위험과 변경 근거 갱신 |
| Req_008 | 직접 영향 | 정상 3회 복귀 및 Fault 재발 조건 반영 |
| Req_011 | 직접 영향 | Fault 원인별 식별 조건 명확화 |
| SWR-SAFE-004/005 | 직접 영향 | Counter 증가·초기화 요구사항 반영 |
| SWR-MON-002 | 직접 영향 | 원인 구분 가능한 FaultCode 요구로 변경 |
| SW-IF-003 | 직접 영향 | CanMonitor→SafetyPolicy 데이터에 FaultCode 반영 |
| SW-IF-005 | 간접 영향 | 제어용 출력 금지 Flag 유지 및 진단 경로 검토 |
| SW-IF-008 | 직접 영향 | 외부 모니터링용 FaultCode 정의 |
| App_CanMonitor.c | 직접 영향 | RTE Read·Alive Counter·Invalid 원인별 비트 설정 |
| App_SafetyPolicy.c | 직접 영향 | CanMonitor와 WdgM FaultCode 통합 |
| ControlCalc / Pwm_Actuator | 회귀 영향 | 출력 차단과 정상 복귀 후 출력 재개 확인 |
| AUTOSAR RTE 설정 | 직접 영향 | Port Data Element 자료형 및 Interface 갱신 |
| 단위·통합·시스템시험 | 직접 영향 | Fault별 코드와 전달·관측 시험 추가 |

## 6. 설계 및 구현 변경

### 6.1 정상 복귀 로직

```c
if (faultDetected == TRUE)
{
    recoveryCount = 0U;
    systemState = FAIL_SAFE;
}
else if (systemState == FAIL_SAFE)
{
    recoveryCount++;

    if (recoveryCount >= 3U)
    {
        recoveryCount = 0U;
        systemState = NORMAL;
    }
}
```

### 6.2 FaultCode 정의

| 값 | Fault 원인 |
|---:|---|
| `0x00` | 정상 |
| `0x01` | RTE Read 실패 |
| `0x02` | Alive Counter 갱신 이상 |
| `0x04` | 조향값 Invalid |
| `0x08` | WdgM FAILED |
| `0x10` | WdgM EXPIRED |
| `0x20` | WdgM STOPPED |

```c
#define FAULT_NONE           (0x00U)
#define FAULT_RTE_READ       (0x01U)
#define FAULT_ALIVE_COUNTER  (0x02U)
#define FAULT_STEER_INVALID  (0x04U)
#define FAULT_WDGM_FAILED    (0x08U)
#define FAULT_WDGM_EXPIRED   (0x10U)
#define FAULT_WDGM_STOPPED   (0x20U)
```

### 6.3 처리 원칙

- CanMonitor는 진단 결과에 해당하는 비트를 설정하여 SafetyPolicy로 전달한다.
- SafetyPolicy는 CanMonitor FaultCode와 WdgM FaultCode를 OR 연산으로 통합한다.
- 통합 FaultCode가 `FAULT_NONE`이 아니면 FAIL-SAFE로 전환한다.
- ControlCalc와 Pwm_Actuator에는 기존 출력 금지 Boolean Flag를 유지하여 변경 범위를 제한한다.
- 통합 FaultCode는 외부 모니터링 Interface로 제공하여 CANoe에서 원인별로 관측한다.

## 7. 검증 변경 및 회귀시험

### 7.1 정상 복귀 시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| UT-SAFE-004 | 정상 조건 1회 | Counter 1, FAIL-SAFE 유지 |
| UT-SAFE-005 | 정상 조건 2회 | Counter 2, FAIL-SAFE 유지 |
| UT-SAFE-006 | 정상 조건 3회 | NORMAL 복귀, Counter 초기화 |
| UT-SAFE-007 | 정상 2회 후 Fault 재발 | Counter 0, FAIL-SAFE 유지 |
| SYS-TC-REC-001 | Fault 해제 후 정상 1·2회 | PWM 0 및 FAIL-SAFE 유지 |
| SYS-TC-REC-002 | 정상 조건 3회 연속 | NORMAL 복귀 및 정상 출력 허용 |
| SYS-TC-REC-003 | 정상 2회 후 Fault 재발 | 복귀 취소 및 FAIL-SAFE 유지 |

### 7.2 FaultCode 시험

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| UT-DIAG-FC-001 | RTE Read 실패 주입 | FaultCode `0x01` 출력 |
| UT-DIAG-FC-002 | 동일 Alive Counter 반복 | FaultCode `0x02` 출력 |
| UT-DIAG-FC-003 | 조향값 범위 이탈 | FaultCode `0x04` 출력 |
| UT-WDG-FC-001 | WdgM FAILED 주입 | FaultCode에 `0x08` 설정 |
| UT-WDG-FC-002 | WdgM EXPIRED 주입 | FaultCode에 `0x10` 설정 |
| UT-WDG-FC-003 | WdgM STOPPED 주입 | FaultCode에 `0x20` 설정 |
| UT-SAFE-FC-001 | 통신 Fault와 WdgM Fault 동시 주입 | 두 Fault 비트 유지 및 FAIL-SAFE 전환 |
| ITC-SW-FC-001 | CanMonitor→SafetyPolicy→Monitoring 연계 | FaultCode가 변형 없이 전달됨 |
| SYS-TC-MON-001 | Timeout·Invalid·WdgM Fault 순차 주입 | CANoe에서 원인별 FaultCode와 상태 구분 |

회귀시험은 정상 조향 출력, Timeout, Invalid, WdgM Fault, PWM 차단, 방향 출력 차단 및 정상 복귀 후 출력 재개를 대상으로 한다.

## 8. 형상 및 버전관리 계획

### 8.1 기준선

```text
BL-01: 정상 1회 복귀 + 단일 Boolean Fault Flag
BL-02: 정상 3회 연속 복귀 + 원인별 FaultCode (검증 완료 기준선)
```

### 8.2 Git 관리 예시

```bash
git checkout -b change/CR-003-safety-diagnostics
git commit -m "CR-003 Update recovery and monitoring requirements"
git commit -m "CR-003 Add recovery counter and fault code"
git commit -m "CR-003 Update AUTOSAR interfaces"
git commit -m "CR-003 Add recovery and fault code tests"
git commit -m "CR-003 Update traceability matrix"
git tag -a BL-02 -m "CR-003 verified safety diagnostics baseline"
```

### 8.3 변경 대상 산출물

| 산출물 | 변경 내용 |
|---|---|
| `00d_HARA_Worksheet.md` | HC-05 변경 근거와 진단성 영향 검토 |
| `01_Requirements.md` | Req_008, Req_011 변경 |
| `02_System_Design.md` | 상태 전이와 Fault 관측 구조 변경 |
| `03_SW_Requirements.md` | SWR-SAFE-004/005, SWR-MON-002 변경 |
| `04_SW_Architecture_Design.md` | SW-IF-003/005/008 및 SafetyPolicy 책임 변경 |
| `05_SW_Detailed_Design_Unit_Construction.md` | Counter·FaultCode·RTE Interface 상세 반영 |
| `06_SW_Unit_Verification.md` | 복귀 경계값 및 Fault별 단위시험 추가 |
| `07_SW_Integration_Verification.md` | FaultCode 전달 및 출력 경로 회귀시험 추가 |
| `08_System_Verification.md` | 복귀·Fault 재발·원인별 모니터링 시험 변경 |
| `Traceability_Matrix.md` | CR-003과 전체 영향 ID 연결 |
| `App_CanMonitor.c` | 원인별 FaultCode 생성 |
| `App_SafetyPolicy.c` | FaultCode 통합 및 모니터링 출력 |
| AUTOSAR RTE 설정 | FaultCode Data Element와 Sender-Receiver Interface 변경 |

## 9. 반영 절차

1. 단일 Boolean Fault Flag와 정상 1회 복귀 상태를 변경 전 기준선 `BL-01`로 확정한다.
2. CR-003을 Open 상태로 등록하고 영향 분석 결과를 승인한다.
3. 요구사항부터 아키텍처·상세설계까지 상위에서 하위 순서로 수정한다.
4. 정상 복귀 로직, FaultCode 코드 및 AUTOSAR Interface 설정을 변경한다.
5. 단위·통합·시스템시험과 회귀시험을 수행한다.
6. 결과와 결함·수정 Commit을 각 Test Case ID에 연결한다.
7. `Traceability_Matrix.md`와 모든 변경 문서의 Version·변경이력을 갱신한다.
8. 검증 완료 후 CR-003을 Closed로 전환하고 `BL-02` Tag를 생성한다.

---

본 문서는 포트폴리오용 변경관리 시나리오의 기준 문서다. 시스템검증과 추적성 검토에서 발견된 문제를 요구사항, 설계, 구현, 시험 및 형상관리까지 전개하는 과정을 재현한다.
