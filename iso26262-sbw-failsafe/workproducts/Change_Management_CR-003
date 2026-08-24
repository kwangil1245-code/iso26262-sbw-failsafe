# 변경관리 시나리오: FAIL-SAFE 정상 복귀 조건 강화

**Change Request ID**: CR-003  
**Title**: FAIL-SAFE 정상 복귀 안정화  
**Version**: 0.1  
**Date**: 2026-08-24  
**Status**: Planned (산출물 반영 전)  
**Project**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템

---

## 1. 변경 목적

초기 설계에서는 Fault가 해제되고 정상 메시지가 1회 수신되면 FAIL-SAFE에서 NORMAL로 즉시 복귀하였다. CAPL 기반 시스템검증에서 정상 메시지와 비정상 메시지가 번갈아 수신되는 간헐적 Fault 조건을 적용한 결과, 시스템 상태가 NORMAL과 FAIL-SAFE 사이에서 반복 전환되고 PWM 출력이 순간적으로 재활성화될 수 있음을 확인한 것으로 설정한다.

이에 복귀 조건을 **정상 1회 확인**에서 **정상 조건 3회 연속 확인**으로 변경하고, 복귀 확인 중 Fault가 재발하면 확인 횟수를 초기화하도록 변경한다.

## 2. 변경요청

| 항목 | 내용 |
|---|---|
| 발견 단계 | 시스템검증 |
| 관련 시험 | SYS-TC-REC-001 |
| 변경 전 | 정상 메시지 1회 수신 후 NORMAL 복귀 |
| 변경 후 | 정상 조건 3회 연속 확인 후 NORMAL 복귀 |
| 변경 사유 | 간헐적 Fault 조건에서 상태 반복 전환 및 출력 재활성화 방지 |
| 안전 영향 | 불안정한 입력 상태에서 조향 출력이 다시 허용될 가능성 |
| 변경 유형 | 안전 상태 전이 요구사항 변경 |

## 3. 변경 전 문제 재현

| 수신 순서 | 변경 전 상태 | 문제점 |
|---|---|---|
| Fault | FAIL-SAFE | 정상 안전 전환 |
| 정상 1회 | NORMAL | 너무 빠른 정상 복귀 |
| Fault 재발 | FAIL-SAFE | 상태 재전환 |
| 정상 1회 | NORMAL | PWM 출력 재활성화 가능 |

## 4. 요구사항 변경안

### 변경 전

> Req_008: Fault가 해제되면 시스템은 FAIL-SAFE 상태에서 NORMAL 상태로 복귀해야 한다.

### 변경 후

> Req_008: 시스템은 FAIL-SAFE 상태에서 Fault가 없는 정상 조건이 3회 연속 확인된 경우에만 NORMAL 상태로 복귀해야 한다. 정상 확인 중 Fault가 다시 발생하면 정상 확인 횟수를 초기화하고 FAIL-SAFE 상태를 유지해야 한다.

### 하위 SW 요구사항

> SWR-SAFE-004: SW는 FAIL-SAFE 상태에서 정상 조건이 확인될 때마다 정상 복귀 Counter를 증가시키고, 3회 연속 확인된 경우 NORMAL 상태로 전환해야 한다.

> SWR-SAFE-005: SW는 정상 복귀 조건 확인 중 Fault가 다시 발생하면 정상 복귀 Counter를 0으로 초기화해야 한다.

## 5. 추적성 및 영향 분석

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

| 대상 | 영향 구분 | 조치 |
|---|---|---|
| HC-05 | 직접 영향 | 복귀 불안정 위험과 구현 조건 갱신 |
| SG-05 | 의미 유지 | 변경 없이 하위 요구사항 영향 검토 기록 |
| Req_008 | 직접 영향 | 연속 3회 복귀 조건 및 Fault 재발 조건 추가 |
| SYS-DES-008 | 직접 영향 | 상태 전이와 복귀 Counter 반영 |
| SWR-SAFE-004/005 | 직접 영향 | Counter 증가·초기화 요구사항 반영 |
| SWC-003 / RUN-003 | 직접 영향 | SafetyPolicy 내부 상태 및 처리 책임 반영 |
| SW-IF-005 | 간접 영향 | Interface 형식 유지 여부 검토 |
| ControlCalc / Pwm_Actuator | 회귀 영향 | FAIL-SAFE 유지 및 정상 복귀 후 출력 재개 확인 |
| Timeout / Invalid 진단 | 영향 없음 | Fault 검출 기준은 변경하지 않음 |

## 6. 설계 및 구현 변경안

| 항목 | 변경 내용 |
|---|---|
| 내부 변수 | `recoveryCount` (`uint8`, 초기값 0) 추가 |
| 상수 | `RECOVERY_THRESHOLD = 3` 추가 |
| 정상 조건 1·2회 | FAIL-SAFE 유지 및 Counter 증가 |
| 정상 조건 3회 | NORMAL 복귀 및 Counter 초기화 |
| 복귀 중 Fault 재발 | Counter 0 초기화 및 FAIL-SAFE 유지 |
| FAIL-SAFE 출력 | PWM Duty 0, 좌·우 방향 출력 비활성 유지 |

```c
if (faultDetected == TRUE)
{
    recoveryCount = 0U;
    systemState = FAIL_SAFE;
}
else if (systemState == FAIL_SAFE)
{
    recoveryCount++;

    if (recoveryCount >= RECOVERY_THRESHOLD)
    {
        recoveryCount = 0U;
        systemState = NORMAL;
    }
}
```

## 7. 검증 변경안

| 시험 ID | 시험 조건 | 기대 결과 |
|---|---|---|
| UT-SAFE-004 | 정상 조건 1회 | Counter 1, FAIL-SAFE 유지 |
| UT-SAFE-005 | 정상 조건 2회 | Counter 2, FAIL-SAFE 유지 |
| UT-SAFE-006 | 정상 조건 3회 | NORMAL 복귀, Counter 초기화 |
| UT-SAFE-007 | 정상 2회 후 Fault 재발 | Counter 0, FAIL-SAFE 유지 |
| SYS-TC-REC-001 | Fault 해제 후 정상 1·2회 | PWM 0 및 FAIL-SAFE 유지 |
| SYS-TC-REC-002 | 정상 조건 3회 연속 | NORMAL 복귀 및 정상 출력 허용 |
| SYS-TC-REC-003 | 정상 2회 후 Fault 재발 | 복귀 취소 및 FAIL-SAFE 유지 |

회귀시험 대상은 정상 조향 출력, Timeout, Invalid, WdgM Fault, PWM 차단 및 방향 출력 차단 시험으로 한다.

## 8. 형상 및 버전관리 계획

### 기준선

```text
BL-01: 정상 1회 수신 후 복귀
BL-02: 정상 조건 3회 연속 확인 후 복귀 (검증 완료 기준선)
```

### Git 관리 예시

```bash
git checkout -b change/CR-003-recovery-debounce
git commit -m "CR-003 Update fail-safe recovery requirement"
git commit -m "CR-003 Add recovery counter to SafetyPolicy"
git commit -m "CR-003 Add recovery and regression tests"
git commit -m "CR-003 Update traceability matrix"
git tag -a BL-02 -m "CR-003 verified recovery baseline"
```

### 산출물 버전 변경 대상

| 산출물 | 변경 내용 |
|---|---|
| `00d_HARA_Worksheet.md` | HC-05 구현 조건 및 변경 근거 |
| `01_Requirements.md` | Req_008 변경 |
| `02_System_Design.md` | SYS-DES-008 상태 전이 변경 |
| `03_SW_Requirements.md` | SWR-SAFE-004/005 변경 |
| `04_SW_Architecture_Design.md` | SafetyPolicy 책임 및 내부 상태 검토 |
| `05_SW_Detailed_Design_Unit_Construction.md` | Counter·임계값·처리 로직 변경 |
| `06_SW_Unit_Verification.md` | 복귀 경계값 시험 변경 및 재수행 |
| `07_SW_Integration_Verification.md` | 관련 출력 경로 회귀시험 |
| `08_System_Verification.md` | 복귀 및 Fault 재발 시험 변경 |
| `Traceability_Matrix.md` | CR-003 및 변경 ID 연결 |

## 9. 향후 실제 반영 절차

1. 현재 문서 세트를 변경 전 기준선 `BL-01`로 확정한다.
2. `CR-003`을 Open 상태로 등록하고 영향 분석 결과를 승인한다.
3. 요구사항부터 상세설계까지 상위→하위 순서로 수정한다.
4. 코드와 AUTOSAR 설정을 변경한다.
5. 단위·통합·시스템 및 회귀시험 결과를 기록한다.
6. `Traceability_Matrix.md`에 `CR-003` 연결을 추가한다.
7. 모든 변경 문서의 Version과 변경이력을 갱신한다.
8. 검증 완료 후 `CR-003`을 Closed로 전환하고 `BL-02` Tag를 생성한다.

---

본 문서는 변경관리 실습을 위한 계획 문서다. 현재 공식 산출물에는 정상 조건 3회 연속 복귀 로직이 이미 반영되어 있으므로, 실제 버전관리 실습 시에는 변경 전 버전을 별도 기준선으로 구성한 후 변경 과정을 순차적으로 재현한다.
