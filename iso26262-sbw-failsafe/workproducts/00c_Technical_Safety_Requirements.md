# 기술 안전 요구사항 명세서 (Technical Safety Requirements Specification)

**Document ID**: STEER-00C-TSR  
**ISO 26262 Reference**: Part 4 (Product Development at the System Level)  
**Version**: 1.1  
**Status**: Draft  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**Subtitle**: Functional Safety Requirement의 기술적 안전 요구사항 전개

---

> 본 문서는 Functional Safety Requirement(FSR)를 기반으로 시스템 수준에서 필요한 Technical Safety Requirement(TSR)를 정의한다.  
> 본 프로젝트는 SW 중심의 교육 프로젝트로 HW Safety Requirement는 범위에서 제외하며, TSR 중 SW로 구현되는 요구사항을 후속 SW Requirement 및 SW Architecture로 구체화한다.

---

## 1. 목적 및 범위

본 문서는 `00b_Functional_Safety_Requirements.md`에서 정의한 FSR을 시스템 수준의 기술 안전 요구사항으로 구체화하는 것을 목적으로 한다.

상위 FSR에서는 다음 안전 기능을 정의하였다.

- 조향 정보 수신 상태 감시 및 이상 감지
- 조향 입력 유효성 감시 및 이상 감지
- 조향 제어 관련 기능의 실행 상태 감시
- 이상 감지 시 안전 상태 전환
- 안전 상태에서 위험한 조향 출력 방지

본 TSR에서는 이러한 기능을 시스템에서 실현하기 위한 **기술적 감시 방법, Fault 전달, 상태 관리 및 출력 제한 방법**을 정의한다.

본 프로젝트의 주요 안전 요구사항 전개 흐름은 다음과 같다.

`Safety Goal → FSR → TSR → SW Requirement → SW Architecture → Detailed Design → Verification`

---

## 2. 요구사항 관리 원칙

본 문서의 TSR은 다음 원칙에 따라 정의한다.

- 각 TSR은 상위 FSR과 추적성을 가져야 한다.
- FSR에서 정의한 안전 기능을 시스템 수준의 기술적 방법으로 구체화한다.
- TSR은 후속 SW 요소에 할당할 수 있는 수준으로 정의한다.
- TSR에서 구체적인 코드 및 내부 변수 수준의 구현은 정의하지 않는다.
- SW로 구현되는 TSR은 후속 SW Requirement로 구체화한다.
- 본 프로젝트에서는 HW Safety Requirement를 별도로 정의하지 않는다.

Alive Counter 사용, 데이터 갱신 감시, 상태 관리 및 출력 비활성화와 같은 **기술적 안전 메커니즘은 TSR에서 정의**한다.

반면 다음과 같은 상세 구현 조건은 후속 SW Requirement 또는 Detailed Design에서 정의한다.

- Alive Counter의 구체적인 비교 연산
- Fault 판정을 위한 연속 발생 횟수
- 정상 복귀를 위한 연속 정상 판정 횟수
- 구체적인 조향각 허용 범위
- PWM Duty 값
- 내부 변수 및 함수
- SWC 내부 알고리즘

---

## 3. 기술 안전 요구사항

| TSR ID | 구분 | Technical Safety Requirement | 상위 FSR |
| :---: | --- | --- | :---: |
| **TSR_001** | 통신 진단 | 수신 ECU는 조향 정보에 포함된 Alive Counter를 이용하여 데이터 갱신 상태를 주기적으로 감시하고, 갱신 이상이 확인될 경우 통신 Fault를 생성해야 한다. | FSR_001 |
| **TSR_002** | 입력 진단 | 수신 ECU는 수신된 조향 입력을 정의된 유효 범위와 비교하여 입력의 유효성을 판단하고, 유효 범위를 벗어난 경우 입력 Fault를 생성해야 한다. | FSR_002 |
| **TSR_003** | 실행 진단 | 수신 ECU는 조향 제어 관련 SW 기능의 주기적 실행 상태를 감시하고, 정상적인 실행이 확인되지 않는 경우 실행 Fault를 생성해야 한다. | FSR_003 |
| **TSR_004** | Fault 관리 | 수신 ECU의 안전 상태 판단 기능은 통신 Fault, 입력 Fault 및 실행 Fault를 수집하고 이를 안전 상태 판단에 반영해야 한다. | FSR_004 |
| **TSR_005** | 상태 관리 | 안전 관련 Fault가 확인된 경우 수신 ECU는 정상 조향 제어 상태에서 Fail-Safe 상태로 전환하고, Fault가 지속되는 동안 Fail-Safe 상태를 유지해야 한다. | FSR_004 |
| **TSR_006** | 안전 출력 | Fail-Safe 상태에서 수신 ECU의 출력 제어 기능은 조향 Actuator에 전달되는 구동 출력을 비활성화하여 위험한 조향 동작을 방지해야 한다. | FSR_005 |
| **TSR_007** | 상태 복귀 | Fail-Safe 진입 원인이 제거된 후 정상 조건이 지속적으로 확인된 경우에만 수신 ECU가 Normal 상태로 복귀하여 조향 구동 출력을 다시 활성화할 수 있어야 한다. | FSR_004, FSR_005 |

---

## 4. FSR별 TSR 전개

### 4.1 FSR_001 — 조향 정보 수신 상태 감시

**Functional Safety Requirement**

> 시스템은 조향 정보가 정상적으로 수신되는지 감시하고, 정상적으로 수신되지 않는 상태를 감지해야 한다.

**Technical Safety Requirement**

**TSR_001**

> 수신 ECU는 조향 정보에 포함된 Alive Counter를 이용하여 데이터 갱신 상태를 주기적으로 감시하고, 갱신 이상이 확인될 경우 통신 Fault를 생성해야 한다.

**전개 관계**

`SG-01 → FSR_001 → TSR_001 → SW 통신 진단 요구사항`

FSR_001에서는 조향 정보의 정상 수신 여부를 감시해야 한다는 기능적 요구사항을 정의한다.

TSR_001에서는 이를 구현하기 위한 기술적 방법으로 **Alive Counter 기반 데이터 갱신 감시**를 정의한다.

Alive Counter의 구체적인 정상 증가 조건과 Fault 판정 로직은 후속 SW Requirement에서 정의한다.

---

### 4.2 FSR_002 — 조향 입력 유효성 감시

**Functional Safety Requirement**

> 시스템은 수신된 조향 입력의 유효성을 판단하고, 비정상적인 조향 입력을 감지해야 한다.

**Technical Safety Requirement**

**TSR_002**

> 수신 ECU는 수신된 조향 입력을 정의된 유효 범위와 비교하여 입력의 유효성을 판단하고, 유효 범위를 벗어난 경우 입력 Fault를 생성해야 한다.

**전개 관계**

`SG-02 → FSR_002 → TSR_002 → SW 입력 진단 요구사항`

FSR_002에서 요구한 입력 유효성 감시 기능을 **범위 기반 입력 진단 방법**으로 구체화한다.

조향 입력의 구체적인 최소·최대 허용값은 후속 SW Requirement에서 정의한다.

---

### 4.3 FSR_003 — 조향 제어 실행 상태 감시

**Functional Safety Requirement**

> 시스템은 조향 제어 관련 기능의 실행 상태를 감시하고, 실행 이상을 감지해야 한다.

**Technical Safety Requirement**

**TSR_003**

> 수신 ECU는 조향 제어 관련 SW 기능의 주기적 실행 상태를 감시하고, 정상적인 실행이 확인되지 않는 경우 실행 Fault를 생성해야 한다.

**전개 관계**

`SG-03 → FSR_003 → TSR_003 → SW 실행 감시 요구사항`

FSR_003의 실행 이상 감지 기능을 **주기적 SW 실행 상태 감시**라는 기술적 방법으로 구체화한다.

구체적인 실행 주기와 실행 이상 판정 기준은 후속 SW Requirement 및 SW 설계에서 정의한다.

---

### 4.4 FSR_004 — 안전 상태 전환

**Functional Safety Requirement**

> 시스템은 조향 정보 수신 이상, 비정상 조향 입력 또는 조향 제어 관련 기능의 실행 이상이 감지된 경우 안전 상태로 전환해야 한다.

FSR_004는 다음 두 개의 TSR로 구체화한다.

#### TSR_004 — Fault 통합 및 안전 상태 판단

> 수신 ECU의 안전 상태 판단 기능은 통신 Fault, 입력 Fault 및 실행 Fault를 수집하고 이를 안전 상태 판단에 반영해야 한다.

#### TSR_005 — Fail-Safe 상태 전환 및 유지

> 안전 관련 Fault가 확인된 경우 수신 ECU는 정상 조향 제어 상태에서 Fail-Safe 상태로 전환하고, Fault가 지속되는 동안 Fail-Safe 상태를 유지해야 한다.

**전개 관계**

```text
SG-01 / SG-02 / SG-03
          ↓
       FSR_004
       /     \
  TSR_004   TSR_005
     ↓          ↓
Fault 통합   상태 전환/유지
       \       /
       SW Safety
       Requirements
```

FSR_004의 기능적 안전 요구사항을 기술적으로 다음과 같이 분리한다.

1. 각 진단 기능에서 생성된 Fault를 안전 상태 판단 기능에 전달한다.
2. 안전 관련 Fault를 통합적으로 판단한다.
3. Fault가 확인되면 Fail-Safe 상태로 전환한다.
4. Fault가 지속되는 동안 Fail-Safe 상태를 유지한다.

---

### 4.5 FSR_005 — 위험한 조향 출력 방지

**Functional Safety Requirement**

> 시스템은 안전 상태에서 운전자 의도와 다른 위험한 조향 출력이 발생하지 않도록 해야 한다.

**Technical Safety Requirement**

**TSR_006**

> Fail-Safe 상태에서 수신 ECU의 출력 제어 기능은 조향 Actuator에 전달되는 구동 출력을 비활성화하여 위험한 조향 동작을 방지해야 한다.

**전개 관계**

`SG-01 / SG-02 / SG-03 → FSR_005 → TSR_006 → SW 안전 출력 요구사항`

FSR_005의 위험 출력 방지 요구사항을 **Actuator 구동 출력 비활성화**라는 기술적 안전 동작으로 구체화한다.

구체적인 PWM Duty 값 및 방향 제어 출력값은 후속 SW Requirement에서 정의한다.

---

## 5. 정상 상태 복귀 요구사항

Fail-Safe 상태에서 Normal 상태로 복귀하면 조향 구동 출력이 다시 활성화될 수 있으므로, 정상 복귀 조건 역시 안전과 관련된 기술 요구사항으로 관리한다.

### TSR_007 — 제한된 정상 상태 복귀

> Fail-Safe 진입 원인이 제거된 후 정상 조건이 지속적으로 확인된 경우에만 수신 ECU가 Normal 상태로 복귀하여 조향 구동 출력을 다시 활성화할 수 있어야 한다.

**관련 상위 요구사항**

`FSR_004, FSR_005 → TSR_007`

정상 상태 복귀는 Fail-Safe 상태를 해제하고 조향 출력을 다시 활성화하는 동작이므로, 일시적인 정상 판단만으로 복귀하지 않도록 제한한다.

구체적인 연속 정상 판정 횟수와 복귀 판단 알고리즘은 후속 SW Requirement에서 정의한다.

---

## 6. FSR-TSR 추적성

| Safety Goal | FSR | TSR | 기술적 실현 방법 |
| :---: | :---: | :---: | --- |
| SG-01 | FSR_001 | TSR_001 | Alive Counter 기반 데이터 갱신 감시 |
| SG-02 | FSR_002 | TSR_002 | 조향 입력 유효 범위 기반 진단 |
| SG-03 | FSR_003 | TSR_003 | SW 기능 실행 상태 감시 |
| SG-01~03 | FSR_004 | TSR_004 | 진단 Fault 수집 및 통합 |
| SG-01~03 | FSR_004 | TSR_005 | Fail-Safe 상태 전환 및 유지 |
| SG-01~03 | FSR_005 | TSR_006 | Actuator 구동 출력 비활성화 |
| SG-01~03 | FSR_004, FSR_005 | TSR_007 | 정상 조건 확인 후 제한된 복귀 |

---

## 7. TSR-SW Requirement 전개 방향

본 프로젝트에서는 HW Safety Requirement를 범위에서 제외하고, TSR을 SW Requirement로 구체화한다.

| TSR | SW Requirement 전개 방향 |
|---|---|
| TSR_001 | Alive Counter 비교 및 통신 Fault 생성 |
| TSR_002 | 조향 입력 범위 검사 및 입력 Fault 생성 |
| TSR_003 | SW 기능 실행 주기 감시 및 실행 Fault 생성 |
| TSR_004 | 각 진단 Fault 수집 및 안전 상태 판단 |
| TSR_005 | Normal / Fail-Safe 상태 전환 및 유지 |
| TSR_006 | Fail-Safe 상태에서 PWM 및 방향 출력 제한 |
| TSR_007 | 연속 정상 조건 확인 및 Normal 상태 복귀 |

---

## 8. 요구사항 구체화 예시

### 통신 이상 감지

```text
Safety Goal
SG-01
위험한 조향 출력 방지
        ↓
Functional Safety Requirement
FSR_001
조향 정보 수신 상태 감시
        ↓
Technical Safety Requirement
TSR_001
Alive Counter 기반 데이터 갱신 감시
        ↓
SW Requirement
Alive Counter 정상 증가 조건 및 Fault 판단
        ↓
SW Architecture
통신 진단 기능
        ↓
Detailed Design
구체적인 비교 및 Fault 생성 로직
```

### 안전 상태 전환 및 출력 차단

```text
FSR_004 / FSR_005
        ↓
TSR_004
Fault 통합
        ↓
TSR_005
Fail-Safe 전환 및 유지
        ↓
TSR_006
Actuator 구동 출력 비활성화
        ↓
SW Requirement
상태 전이 및 출력 제한 조건
        ↓
SW Architecture
안전 상태 판단 / 출력 제어 기능
```

---

## 9. 기술 상세 구체화 원칙

다음 항목은 TSR의 기술적 안전 메커니즘을 후속 SW Requirement 및 SW 설계에서 추가로 구체화한다.

- Alive Counter의 정상 증가 조건
- Alive Counter wrap-around 처리
- Fault 판정을 위한 연속 발생 횟수
- 조향 입력의 구체적인 유효 범위
- SW 실행 주기 및 실행 이상 판정 기준
- 정상 상태 복귀를 위한 연속 정상 판정 횟수
- 안전 상태에서의 PWM Duty 값
- 조향 방향 출력 활성/비활성 값
- AUTOSAR SWC별 기능 할당
- RTE 인터페이스
- Runnable 및 Task 구성
- 내부 변수 및 상세 알고리즘

이를 통해 안전 요구사항을 다음과 같이 단계적으로 구체화한다.

`Hazardous Event → Safety Goal → FSR → TSR → SW Requirement → SW Architecture → Detailed Design → Verification`
