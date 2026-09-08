# 기술 안전 요구사항 명세서 (Technical Safety Requirements Specification)

**Document ID**: STEER-00C-TSR  
**ISO 26262 Reference**: Part 4 (Product Development at the System Level)  
**Version**: 1.0  
**Date**: 2026-09-08  
**Status**: Draft  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**Subtitle**: 조향 관련 Functional Safety Requirement의 기술적 안전 요구사항 전개

---

> 본 문서는 Functional Safety Requirement(FSR)를 만족하기 위해 시스템에 필요한 Technical Safety Requirement(TSR)를 정의한다.
> TSR은 안전 기능을 시스템 요소에 기술적으로 할당하기 위한 요구사항이며, 구체적인 SW 내부 구조, AUTOSAR SWC, Fault 판정 횟수, 조향각 수치 범위 및 출력값은 후속 SW 요구사항과 SW 설계 단계에서 구체화한다.

## 1. 목적 및 범위

본 문서는 `00b_Functional_Safety_Requirements.md`에서 정의한 FSR을 기술적 안전 요구사항으로 구체화하는 것을 목적으로 한다.

안전 요구사항은 다음과 같이 전개한다.

`Safety Goal → FSR → TSR → System/SW Requirement → SW Architecture → Detailed Design → Verification`

본 프로젝트는 SW 중심의 교육 프로젝트이므로 별도의 HW Safety Requirement는 정의하지 않으며, 본 문서에서 정의한 TSR 중 SW로 구현되는 요구사항은 후속 SW 요구사항 및 SW Architecture로 할당한다.

---

# Steering System TSRs

## Technical Safety Requirement: Steering Data Update Monitoring `TSR_001`

**status:** open  
**asil:** D  
**derives from:** `FSR_001`  
**allocated to:** Steering Information Reception / Diagnostic Function  
**implemented by:** `Req_003`, `SWR-DIAG-001~004`

---

시스템은 수신된 조향 정보가 최신 정보인지 판단할 수 있도록 데이터 갱신 상태를 감시할 수 있는 기술적 수단을 제공해야 한다.

---

**ASIL: D**

---

## Technical Safety Requirement: Steering Input Validity Monitoring `TSR_002`

**status:** open  
**asil:** D  
**derives from:** `FSR_002`  
**allocated to:** Steering Input Diagnostic Function  
**implemented by:** `Req_004`, `SWR-DIAG-005~008`

---

시스템은 수신된 조향 입력이 정의된 유효 조건을 만족하는지 판단하고, 유효하지 않은 조향 입력을 식별할 수 있는 진단 기능을 제공해야 한다.

---

**ASIL: D**

---

## Technical Safety Requirement: Steering Control Execution Supervision `TSR_003`

**status:** open  
**asil:** D  
**derives from:** `FSR_003`  
**allocated to:** Steering Control Execution Monitoring Function  
**implemented by:** `Req_005`, `SWR-EXEC-001~003`

---

시스템은 조향 제어 관련 기능이 의도된 실행 상태를 유지하는지 감시하고, 실행 이상을 식별할 수 있는 실행 감시 기능을 제공해야 한다.

---

**ASIL: D**

---

## Technical Safety Requirement: Fault Status Integration `TSR_004`

**status:** open  
**asil:** D  
**derives from:** `FSR_004`  
**allocated to:** Safety State Decision Function  
**implemented by:** `Req_006`, `SWR-SAFE-001`

---

시스템은 조향 정보 수신 이상, 조향 입력 이상 및 조향 제어 실행 이상에 대한 진단 결과를 안전 상태 판단 기능에 전달하고 통합적으로 판단할 수 있어야 한다.

---

**ASIL: D**

---

## Technical Safety Requirement: Safe State Transition `TSR_005`

**status:** open  
**asil:** D  
**derives from:** `FSR_004`  
**allocated to:** Safety State Management Function  
**implemented by:** `Req_006`, `SWR-SAFE-001`

---

시스템은 안전 관련 Fault가 확인된 경우 정상 조향 제어 상태에서 안전 상태로 전환할 수 있는 상태 관리 기능을 제공해야 한다.

---

**ASIL: D**

---

## Technical Safety Requirement: Safe Steering Output Limitation `TSR_006`

**status:** open  
**asil:** D  
**derives from:** `FSR_005`  
**allocated to:** Steering Output Control Function  
**implemented by:** `Req_007`, `SWR-SAFE-002~004`

---

시스템은 안전 상태에서 조향 구동 출력이 위험한 조향 동작으로 이어지지 않도록 출력 제어 기능을 제한하거나 비활성화할 수 있어야 한다.

---

**ASIL: D**

---

## Technical Safety Requirement: Safe State Maintenance `TSR_007`

**status:** open  
**asil:** D  
**derives from:** `FSR_005`  
**allocated to:** Safety State Management Function  
**implemented by:** `Req_007`, `SWR-SAFE-005`

---

시스템은 안전 관련 Fault가 지속되는 동안 안전 상태와 안전 출력 조건을 유지해야 한다.

---

**ASIL: D**

---

## Technical Safety Requirement: Controlled Recovery `TSR_008`

**status:** open  
**asil:** D  
**derives from:** System Derived Requirement  
**allocated to:** Safety State Management Function  
**implemented by:** `Req_008`, `SWR-REC-001~004`

---

시스템은 안전 상태 진입 원인이 제거된 이후 정의된 정상 조건이 지속적으로 확인된 경우에만 정상 조향 제어 상태로 복귀할 수 있어야 한다.

---

**ASIL: D**

---

## 2. FSR-TSR 추적성

| FSR ID | TSR ID | 기술적 전개 |
|---|---|---|
| FSR_001 | TSR_001 | 조향 정보 갱신 상태 감시 기능 |
| FSR_002 | TSR_002 | 조향 입력 유효성 진단 기능 |
| FSR_003 | TSR_003 | 조향 제어 실행 상태 감시 기능 |
| FSR_004 | TSR_004 | Fault 진단 결과 통합 |
| FSR_004 | TSR_005 | 안전 상태 전환 |
| FSR_005 | TSR_006 | 안전 조향 출력 제한 |
| FSR_005 | TSR_007 | Fault 지속 시 안전 상태 유지 |
| System Derived | TSR_008 | 정상 상태로의 제한된 복귀 |

---

## 3. TSR-System/SW Requirement 추적성

| TSR ID | System Requirement | SW Requirement | 역할 |
|---|---|---|---|
| TSR_001 | Req_003 | SWR-DIAG-001~004 | 조향 정보 갱신 상태 감시 및 Fault 전달 |
| TSR_002 | Req_004 | SWR-DIAG-005~008 | 조향 입력 유효성 검사 및 Fault 전달 |
| TSR_003 | Req_005 | SWR-EXEC-001~003 | 조향 제어 기능 실행 상태 감시 |
| TSR_004 | Req_006 | SWR-SAFE-001 | 각 진단 Fault를 안전 상태 판단에 반영 |
| TSR_005 | Req_006 | SWR-SAFE-001 | Fault 발생 시 안전 상태 전환 |
| TSR_006 | Req_007 | SWR-SAFE-002~004 | 안전 상태에서 조향 출력 제한 |
| TSR_007 | Req_007 | SWR-SAFE-005 | Fault 지속 중 안전 상태 및 출력 유지 |
| TSR_008 | Req_008 | SWR-REC-001~004 | 정상 조건 확인 후 정상 상태 복귀 |

---

## 4. SW Architecture 전개 방향

| TSR ID | SW Architecture 전개 방향 |
|---|---|
| TSR_001 | 조향 정보 갱신 상태를 감시하고 진단 결과를 생성하는 SW 기능 |
| TSR_002 | 조향 입력 유효성을 판단하고 진단 결과를 생성하는 SW 기능 |
| TSR_003 | 조향 제어 기능의 실행 상태를 감시하는 SW 기능 |
| TSR_004 | 각 진단 결과를 수집하여 안전 상태 판단에 제공하는 인터페이스 |
| TSR_005 | 정상 상태와 안전 상태 간 상태 전이를 관리하는 SW 기능 |
| TSR_006 | 안전 상태에서 조향 구동 출력을 제한하는 SW 기능 |
| TSR_007 | Fault 지속 여부에 따라 안전 상태와 출력 제한을 유지하는 SW 기능 |
| TSR_008 | 정상 조건의 지속 여부를 판단하고 정상 상태 복귀를 관리하는 SW 기능 |

---

## 5. 요구사항 전개 예시

### 통신 이상

`SG-01`
→ `FSR_001`
→ `TSR_001`
→ `Req_003`
→ `SWR-DIAG-001~004`
→ SW Architecture의 조향 정보 진단 기능
→ Detailed Design
→ Test Case

### 안전 상태 전환

`SG-01 / SG-02 / SG-03`
→ `FSR_004`
→ `TSR_004, TSR_005`
→ `Req_006`
→ `SWR-SAFE-001`
→ SW Architecture의 안전 상태 판단 기능
→ Detailed Design
→ Test Case

### 위험 출력 방지

`SG-01 / SG-02 / SG-03`
→ `FSR_005`
→ `TSR_006, TSR_007`
→ `Req_007`
→ `SWR-SAFE-002~005`
→ SW Architecture의 안전 출력 제어 기능
→ Detailed Design
→ Test Case

---

## 6. 기술 상세 구체화 원칙

본 TSR에서는 기술적 안전 기능의 목적과 할당 방향을 정의하며 다음과 같은 상세 조건은 후속 SW 요구사항 및 설계에서 정의한다.

- Alive Counter를 이용한 데이터 갱신 판단 방법
- Fault 판정을 위한 연속 발생 횟수
- 정상 복귀를 위한 연속 정상 판정 횟수
- 조향 입력의 구체적인 유효 범위
- 안전 상태에서의 PWM 출력값
- 조향 방향 출력 활성/비활성 조건
- AUTOSAR SWC 및 RTE 인터페이스 구성
- Watchdog 및 실행 감시 모듈의 구체적인 구성

이를 통해 FSR에서 정의한 안전 기능을 TSR에서 기술적 기능으로 구체화하고, 이후 SW Requirement와 SW Architecture에서 실제 SW 동작과 구조로 전개한다.
