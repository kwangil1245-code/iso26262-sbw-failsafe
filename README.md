# AUTOSAR 기반 조향 Fail-Safe 시스템

AUTOSAR Classic 기반으로 조향 입력과 ECU 내부 실행 이상을 진단하고, Fault 발생 시 **FAIL-SAFE 상태로 전환하여 조향 출력을 차단**하는 프로젝트입니다.

## 주요 기능

- Alive Counter 기반 CAN Timeout 진단
- 조향각 유효 범위(`-512~511`) 검사
- WdgM 기반 ECU 내부 실행 상태 감시
- Fault 발생 시 PWM 및 방향 출력 차단
- 정상 조건 3회 연속 확인 후 NORMAL 상태 복귀

## 개발 및 검증 흐름

```text
HARA → 요구사항 → 시스템·SW 설계 → 구현
     → 단위검증 → 통합검증 → 시스템검증
