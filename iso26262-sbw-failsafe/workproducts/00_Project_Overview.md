# 프로젝트 개요 (Project Overview)

**Document ID**: STEER-00-OV  
**Version**: 1.3  
**Date**: 2026-08-24  
**Status**: Baseline  
**Project Title**: AUTOSAR 기반 조향 관련 오류에 대한 복구 및 진단 시스템  
**Subtitle**: CAN 입력 감시, WdgM 실행 감시 및 FAIL-SAFE 기반 조향 출력 차단

---

## 1. 문서 목적

본 문서는 제출본 전체(00–07)의 공통 배경을 한 페이지로 요약한다. 프로젝트 목적, 핵심 시나리오, 구현 범위, 안전 설계 규칙 및 문서 구성을 간결하게 정리한다.

## 2. 프로젝트 개요

| 항목 | 내용 |
|---|---|
| 개발 목적 | AUTOSAR 기반으로 조향 입력 이상과 ECU 내부 실행 이상을 실시간 진단하고, 이상 발생 시 안전 상태로 전환하여 조향 출력을 제한한다. |
| 시스템 범위 | MPC-5606B 기반 입력·출력 ECU, CAN 통신, AUTOSAR Classic SWC/RTE, WdgM, IoHwAb 및 PWM 하드웨어 출력을 포함한다. |
| 핵심 입력 | 가변저항 기반 조향각, Alive Counter, RTE 수신 결과, WdgM Global Status |
| 핵심 처리 | 조향각 유효 범위 검사, Alive Counter 기반 Timeout 검출, WdgM 실행 이상 판정, NORMAL/FAIL-SAFE 상태 전이, 조향 변화량 기반 방향 및 PWM Duty 계산 |
| 핵심 출력 | 좌·우 방향 신호, PWM Duty, 구동/정지 상태, 정지 표시 LED |
| 검증 범위 | CAN 데이터 송수신, SWC 인터페이스 연결, 상태 전이, PWM·방향·LED 하드웨어 출력을 Unit/Integration/System 수준에서 검증한다. |

## 3. 핵심 시나리오

| 시나리오 | 설명 | 관련 문서 |
|---|---|---|
| 정상 조향 제어 | 입력 ECU가 조향각과 Alive Counter를 10 ms 주기로 전송하면 출력 ECU가 입력을 검증하고 조향 변화량에 따라 방향과 PWM Duty를 계산한다. | `01`, `03`, `04`, `07` |
| CAN Timeout 진단 | 동일한 Alive Counter가 2회 이상 연속 수신되거나 RTE 수신에 실패하면 CanMonitor가 입력 Fault를 생성한다. | `00d`, `01`, `03`, `05`, `07` |
| Invalid 조향각 진단 | 수신 조향각이 유효 범위 `-512–511`을 벗어나면 Invalid Fault를 생성하고 해당 입력을 제어에 사용하지 않는다. | `00d`, `01`, `03`, `05`, `07` |
| WdgM 실행 이상 진단 | WdgM Global Status가 FAILED, EXPIRED 또는 STOPPED이면 ECU 내부 실행 Fault로 판정한다. | `00d`, `01`, `03`, `05`, `07` |
| FAIL-SAFE 전환 | 입력 Fault 또는 WdgM Fault가 발생하면 조향각과 PWM Duty를 0으로 설정하고 좌·우 출력을 차단한다. | `00d`, `01`, `04`, `06`, `07` |
| 안전한 정상 복귀 | FAIL-SAFE 상태에서 Fault가 없는 정상 조건이 3회 연속 확인된 경우에만 NORMAL 상태로 복귀한다. | `00d`, `01`, `04`, `06`, `07` |

## 4. 핵심 용어

| 용어 | 설명 |
|---|---|
| Alive Counter | CAN 메시지가 주기적으로 갱신되고 있는지 확인하기 위해 송신 시마다 증가시키는 카운터 값이다. |
| Timeout | CAN 메시지 또는 Alive Counter가 정상적으로 갱신되지 않아 입력 데이터가 오래된 상태로 판단되는 진단 결과이다. |
| Invalid | 수신한 조향각이 정의된 유효 범위를 벗어나 제어에 사용할 수 없는 상태이다. |
| WdgM | AUTOSAR Watchdog Manager로, Runnable 또는 Task의 실행 상태를 감시하는 안전 메커니즘이다. |
| NORMAL | 입력과 내부 실행 상태가 정상이며 조향 제어 출력이 허용되는 시스템 상태이다. |
| FAIL-SAFE | Fault 발생 시 조향 출력으로 인한 추가 위험을 방지하기 위해 PWM과 방향 출력을 제한하는 안전 상태이다. |
| IoHwAb | AUTOSAR 애플리케이션과 하드웨어 입출력 사이를 추상화하여 PWM, 방향 신호 및 LED 출력을 연결하는 계층이다. |

## 5. 설계 규칙

| 규칙 | 내용 |
|---|---|
| CAN 송신 주기 | SWC_SteeringSensor는 조향각과 Alive Counter를 포함한 메시지를 10 ms 주기로 송신한다. |
| CAN 메시지 형식 | 조향 입력 메시지는 CAN ID `0x100`, DLC `3 Byte`를 사용한다. |
| 조향각 유효 범위 | `-512–511` 범위의 값만 정상 조향 입력으로 인정한다. |
| Timeout 판정 | 최초 정상 수신 이후 동일한 Alive Counter가 2회 이상 연속 수신되면 입력 Fault로 판정한다. |
| WdgM Fault 판정 | Global Status가 FAILED, EXPIRED 또는 STOPPED이면 내부 실행 Fault로 판정한다. |
| FAIL-SAFE 출력 | FAIL-SAFE 상태에서는 조향각과 PWM Duty를 0으로 설정하고 좌·우 방향 및 구동 출력을 비활성화한다. |
| 복귀 기준 | Fault가 없는 정상 조건이 3회 연속 확인된 경우에만 NORMAL 상태로 복귀한다. |
| 계층 분리 | 입력, 진단, 안전 판단, 제어 계산 및 하드웨어 출력을 독립 SWC로 분리하고 RTE 인터페이스로 연결한다. |

## 5-1. 상태 전이 매트릭스

| 현재 상태 | 입력 조건 | 다음 상태 | 출력 동작 |
|---|---|---|---|
| NORMAL | 입력 Fault 없음, WdgM 정상 | NORMAL | 검증된 조향각을 이용해 방향과 PWM Duty를 계산한다. |
| NORMAL | Timeout, Invalid 또는 WdgM Fault 발생 | FAIL-SAFE | 조향각 0, PWM Duty 0, 좌·우 및 구동 출력 비활성, 정지 LED 활성 |
| FAIL-SAFE | 정상 조건 1회 또는 2회 확인 | FAIL-SAFE | 안전 출력을 유지하고 정상 복귀 카운터를 증가시킨다. |
| FAIL-SAFE | 정상 조건 3회 연속 확인 | NORMAL | 복귀 카운터를 초기화하고 정상 조향 출력을 다시 허용한다. |
| FAIL-SAFE | 정상 확인 중 Fault 재발 | FAIL-SAFE | 복귀 카운터를 0으로 초기화하고 안전 출력을 유지한다. |

## 6. 구현 범위와 기대 가치

| 구분 | 내용 |
|---|---|
| 구현 범위 | SWC_SteeringSensor, SWC_CanMonitor, SWC_SafetyPolicy, SWC_ControlCalc, SWC_Pwm_Actuator와 CAN/RTE/IoHwAb 연동 |
| 안전성 가치 | 조향 입력과 ECU 내부 실행 상태를 통합 감시하고 Fault 발생 시 출력 차단 기준을 일관되게 적용한다. |
| 진단 가치 | Timeout, Invalid 및 WdgM 상태를 구분하여 Fault 원인 분석과 안전 상태 판단의 근거를 제공한다. |
| 구조적 가치 | 기능을 독립 SWC로 분리하고 Port/Interface/Runnable/Task Mapping을 통해 AUTOSAR 기반 통합 구조를 구성한다. |
| 검증 가치 | HARA, 요구사항, 설계, 구현 및 시험 ID를 연결하여 요구사항부터 검증 결과까지 추적 가능한 근거를 유지한다. |

## 7. 문서 구성 (00–07)

| 문서 | 역할 |
|---|---|
| `00_Project_Overview.md` | 프로젝트 목적, 핵심 시나리오, 설계 규칙 및 문서 구성 요약 |
| `00d_HARA_Worksheet.md` | Hazardous Event, S/E/C, ASIL Candidate 및 Safety Goal 정의 |
| `01_Requirements.md` | 시스템 요구사항 정의 (`What`) 및 HARA 추적성 |
| `02_System_Design.md` | 시스템 구조, 기능 할당, 데이터 흐름, 통신, 인터페이스, 변수 및 상태 전이 통합 설계 |
| `03_SW_Requirements.md` | 시스템 요구사항에서 도출한 SW 요구사항 정의 |
| `0301_SW_Architecture_Design.md` | SWC, Interface 및 Runnable 소프트웨어 아키텍처 정의 |
| `04_SW_Detailed_Design_Unit_Construction.md` | SW 상세설계 및 Unit 구현 명세 |
| `05_SW_Unit_Verification.md` | SW 단위검증 명세 및 수행 결과 |
| `06_SW_Integration_Verification.md` | Host Stub/Mock 기반 SW 통합검증 명세 및 수행 결과 |
| `07_System_Verification.md` | 입력 ECU 기능 확인 및 CAPL Restbus 기반 시스템 요구사항 검증 |
| `Traceability_Matrix.md` | Safety Goal부터 요구사항, 설계, 구현 및 시험 결과까지 End-to-End 추적성 관리 |
