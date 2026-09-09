# 기술 안전 요구사항 명세서 (Technical Safety Requirements Specification)

**Document ID**: STEER-00C-TSR  
**ISO 26262 Reference**: Part 4 (Product Development at the System Level)  
**Version**: 1.1  
**Status**: Draft  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**Subtitle**: Functional Safety Requirement의 기술적 안전 요구사항 전개

---

> 본 문서는 Functional Safety Concept에서 정의한 Functional Safety Requirement(FSR)를 기반으로, 시스템 수준에서 필요한 Technical Safety Requirement(TSR)를 정의한다.  
> 본 프로젝트는 SW 중심의 교육 프로젝트로 HW Safety Requirement는 범위에서 제외하며, TSR 중 SW로 구현되는 요구사항을 후속 SW Requirement 및 SW Architecture로 구체화한다.

---

## 1. 목적 및 범위

본 문서는 `00b_Functional_Safety_Requirements.md`에서 정의한 FSR을 시스템 수준의 기술 안전 요구사항으로 구체화하는 것을 목적으로 한다.

상위 FSR에서는 다음과 같은 안전 기능을 정의하였다.

- 조향 정보 수신 상태 감시 및 이상 감지
- 조향 입력 유효성 감시 및 이상 감지
- 조향 제어 관련 기능의 실행 상태 감시
- 이상 감지 시 안전 상태 전환
- 안전 상태에서 위험한 조향 출력 방지

본 TSR에서는 이러한 기능을 시스템에서 실현하기 위해 필요한 **기술적 기능과 안전 동작을 정의한다.**

구체적인 SW 내부 로직, Fault 판정 횟수, 조향각 유효 범위, PWM 출력값, AUTOSAR SWC 및 RTE 인터페이스 등은 후속 SW Requirement와 SW 설계 단계에서 구체화한다.

안전 요구사항의 주요 전개 흐름은 다음과 같다.

`Safety Goal → FSR → TSR → SW Requirement → SW Architecture → Detailed Design → Verification`

---

## 2. 요구사항 관리 원칙

본 문서의 TSR은 FSR에서 정의한 기능적 안전 요구사항을 시스템 수준의 기술적 요구사항으로 구체화한다.

- 각 TSR은 하나 이상의 FSR에 추적되어야 한다.
- TSR은 시스템이 갖추어야 할 기술적 안전 기능을 정의한다.
- SW 내부의 구체적인 구현 방법은 TSR에서 정의하지 않는다.
- SW로 구현되는 TSR은 후속 SW Requirement로 구체화한다.
- 본 프로젝트에서는 HW Safety Requirement를 별도로 정의하지 않는다.
- 여러 FSR에서 공통으로 사용되는 안전 상태 관리 및 출력 제한 기능은 공유할 수 있다.

---

## 3. 기술 안전 요구사항

| TSR ID | 구분 | 요약 | Technical Safety Requirement | 상위 FSR |
| :---: | --- | --- | --- | :---: |
| **TSR_001** | 통신 진단 | 조향 정보 갱신 상태 감시 | 시스템은 수신된 조향 정보가 최신 정보인지 판단할 수 있도록 데이터 갱신 상태를 감시하고 이상을 식별할 수 있어야 한다. | FSR_001 |
| **TSR_002** | 입력 진단 | 조향 입력 유효성 판단 | 시스템은 수신된 조향 입력이 정의된 유효 조건을 만족하는지 판단하고 유효하지 않은 입력을 식별할 수 있어야 한다. | FSR_002 |
| **TSR_003** | 실행 진단 | 조향 제어 실행 상태 감시 | 시스템은 조향 제어 관련 기능이 의도된 실행 상태를 유지하는지 감시하고 실행 이상을 식별할 수 있어야 한다. | FSR_003 |
| **TSR_004** | Fault 관리 | 진단 결과 통합 | 시스템은 각 진단 기능에서 식별된 안전 관련 Fault를 안전 상태 판단에 반영할 수 있어야 한다. | FSR_004 |
| **TSR_005** | 상태 관리 | 안전 상태 전환 및 유지 | 시스템은 안전 관련 Fault가 확인된 경우 안전 상태로 전환하고 Fault가 지속되는 동안 안전 상태를 유지해야 한다. | FSR_004 |
| **TSR_006** | 안전 출력 | 위험한 조향 출력 제한 | 시스템은 안전 상태에서 조향 구동 출력이 위험한 조향 동작으로 이어지지 않도록 출력을 제한하거나 비활성화해야 한다. | FSR_005 |
| **TSR_007** | 상태 복귀 | 제한된 정상 상태 복귀 | 시스템은 안전 상태 진입 원인이 제거되고 정의된 정상 조건이 지속적으로 확인된 경우에만 정상 조향 제어 상태로 복귀해야 한다. | FSR_004, FSR_005 |

---

## 4. FSR별 TSR 전개

### 4.1 FSR_001 — 조향 정보 수신 상태 감시

**Functional Safety Requirement**

> 시스템은 조향 정보가 정상적으로 수신되는지 감시하고, 정상적으로 수신되지 않는 상태를 감지해야 한다.

**Technical Safety Requirement — TSR_001**

> 시스템은 수신된 조향 정보가 최신 정보인지 판단할 수 있도록 데이터 갱신 상태를 감시하고 이상을 식별할 수 있어야 한다.

**전개 관계**

`SG-01 → FSR_001 → TSR_001 → SW 진단 요구사항`

TSR_001에서는 데이터 갱신 여부를 판단할 수 있는 기술적 진단 기능을 요구한다.

Alive Counter의 사용 여부, 정상 증가 조건, Fault 판정 횟수 등 구체적인 진단 알고리즘은 후속 SW Requirement에서 정의한다.

---

### 4.2 FSR_002 — 조향 입력 유효성 감시

**Functional Safety Requirement**

> 시스템은 수신된 조향 입력의 유효성을 판단하고, 비정상적인 조향 입력을 감지해야 한다.

**Technical Safety Requirement — TSR_002**

> 시스템은 수신된 조향 입력이 정의된 유효 조건을 만족하는지 판단하고 유효하지 않은 입력을 식별할 수 있어야 한다.

**전개 관계**

`SG-02 → FSR_002 → TSR_002 → SW 진단 요구사항`

TSR_002에서는 조향 입력의 유효성을 판단하는 기술적 진단 기능을 요구한다.

구체적인 조향각 허용 범위 및 Fault 판정 조건은 후속 SW Requirement에서 정의한다.

---

### 4.3 FSR_003 — 조향 제어 실행 상태 감시

**Functional Safety Requirement**

> 시스템은 조향 제어 관련 기능의 실행 상태를 감시하고, 실행 이상을 감지해야 한다.

**Technical Safety Requirement — TSR_003**

> 시스템은 조향 제어 관련 기능이 의도된 실행 상태를 유지하는지 감시하고 실행 이상을 식별할 수 있어야 한다.

**전개 관계**

`SG-03 → FSR_003 → TSR_003 → SW 실행 감시 요구사항`

TSR_003에서는 조향 제어 관련 기능의 정상적인 실행 여부를 확인할 수 있는 기술적 감시 기능을 요구한다.

구체적인 실행 주기 및 실행 이상 판단 방법은 후속 SW Requirement와 SW 설계에서 정의한다.

---

### 4.4 FSR_004 — 안전 상태 전환

**Functional Safety Requirement**

> 시스템은 조향 정보 수신 이상, 비정상 조향 입력 또는 조향 제어 관련 기능의 실행 이상이 감지된 경우 안전 상태로 전환해야 한다.

**Technical Safety Requirement — TSR_004**

> 시스템은 각 진단 기능에서 식별된 안전 관련 Fault를 안전 상태 판단에 반영할 수 있어야 한다.

**Technical Safety Requirement — TSR_005**

> 시스템은 안전 관련 Fault가 확인된 경우 안전 상태로 전환하고, Fault가 지속되는 동안 안전 상태를 유지해야 한다.

**전개 관계**

```text
SG-01 / SG-02 / SG-03
          ↓
       FSR_004
       ↙     ↘
  TSR_004   TSR_005
       ↘     ↙
   SW Safety State Requirement
```

FSR_004의 **이상 발생 시 안전 상태로 전환**이라는 기능 요구사항을 다음과 같이 기술적으로 구체화한다.

1. 진단 결과를 안전 상태 판단에 반영한다.
2. 안전 관련 Fault가 확인되면 안전 상태로 전환한다.
3. Fault가 지속되는 동안 안전 상태를 유지한다.

구체적인 상태 변수, 상태 전이 조건 및 Fault 관리 로직은 후속 SW Requirement와 SW 설계에서 정의한다.

---

### 4.5 FSR_005 — 위험한 조향 출력 방지

**Functional Safety Requirement**

> 시스템은 안전 상태에서 운전자 의도와 다른 위험한 조향 출력이 발생하지 않도록 해야 한다.

**Technical Safety Requirement — TSR_006**

> 시스템은 안전 상태에서 조향 구동 출력이 위험한 조향 동작으로 이어지지 않도록 출력을 제한하거나 비활성화해야 한다.

**전개 관계**

`SG-01 / SG-02 / SG-03 → FSR_005 → TSR_006 → SW 안전 출력 요구사항`

TSR_006에서는 안전 상태에서 실제 조향 구동 명령이 위험한 조향 동작으로 이어지지 않도록 출력 제어 기능을 제한하는 기술적 안전 동작을 정의한다.

구체적인 PWM 출력값 및 조향 방향 출력 조건은 후속 SW Requirement에서 정의한다.

---

## 5. 정상 상태 복귀 요구사항

안전 상태에서 정상 상태로의 복귀는 잘못된 복귀로 인해 위험한 조향 출력이 다시 활성화될 수 있으므로 별도의 기술 안전 요구사항으로 관리한다.

### TSR_007 — 제한된 정상 상태 복귀

> 시스템은 안전 상태 진입 원인이 제거되고 정의된 정상 조건이 지속적으로 확인된 경우에만 정상 조향 제어 상태로 복귀해야 한다.

**관련 상위 요구사항**

`FSR_004, FSR_005 → TSR_007`

TSR_007은 안전 상태 전환 및 위험 출력 방지 기능을 유지하면서 정상 상태로 안전하게 복귀하기 위해 추가적으로 도출된 기술 요구사항이다.

정상 상태 복귀에 필요한 연속 정상 판정 횟수 등 구체적인 복귀 조건은 후속 SW Requirement에서 정의한다.

---

## 6. FSR-TSR 추적성

| Safety Goal | FSR | TSR | 역할 |
| :---: | :---: | :---: | --- |
| SG-01 | FSR_001 | TSR_001 | 조향 정보 갱신 상태 감시 |
| SG-02 | FSR_002 | TSR_002 | 조향 입력 유효성 판단 |
| SG-03 | FSR_003 | TSR_003 | 조향 제어 실행 상태 감시 |
| SG-01~03 | FSR_004 | TSR_004 | 안전 관련 Fault 통합 |
| SG-01~03 | FSR_004 | TSR_005 | 안전 상태 전환 및 유지 |
| SG-01~03 | FSR_005 | TSR_006 | 위험한 조향 출력 제한 |
| SG-01~03 | FSR_004, FSR_005 | TSR_007 | 제한된 정상 상태 복귀 |

---

## 7. SW Requirement 전개 방향

본 프로젝트에서는 HW Safety Requirement를 범위에서 제외하고, TSR 중 SW로 구현되는 안전 기능을 SW Requirement로 구체화한다.

| TSR | SW Requirement 전개 방향 |
|---|---|
| TSR_001 | 조향 정보 갱신 상태 판단 및 통신 이상 진단 |
| TSR_002 | 조향 입력 범위 및 유효성 판단 |
| TSR_003 | 조향 제어 관련 기능의 실행 상태 감시 |
| TSR_004 | 각 진단 Fault 수집 및 안전 상태 판단 |
| TSR_005 | Normal / Fail-Safe 상태 전환 및 유지 |
| TSR_006 | Fail-Safe 상태의 조향 구동 출력 제한 |
| TSR_007 | 정상 조건 확인 및 Normal 상태 복귀 |

---

## 8. 기술 상세 구체화 원칙

다음 항목은 TSR에서 직접 정의하지 않고 후속 SW Requirement 및 SW 설계에서 구체화한다.

- Alive Counter 기반 데이터 갱신 판단 방법
- Alive Counter의 정상 증가 조건
- Fault 판정을 위한 연속 발생 횟수
- 조향 입력의 구체적인 유효 범위
- 정상 상태 복귀를 위한 연속 정상 판정 횟수
- 안전 상태에서의 PWM 출력값
- 조향 방향 출력 활성/비활성 조건
- AUTOSAR SWC별 기능 할당
- RTE 인터페이스
- Runnable 및 Task 구성

이를 통해 안전 요구사항을 다음과 같이 단계적으로 구체화한다.

`Hazardous Event → Safety Goal → FSR → TSR → SW Requirement → SW Architecture → Detailed Design → Verification`
