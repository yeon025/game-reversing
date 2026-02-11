# game-reversing
지뢰찾기, 피카츄 배구를 분석하고 게임 핵을 만든 프로젝트입니다.

<br>

## 📅 프로젝트 기간
2023년 9월 ~ 2023년 12월

<br>

## 🛠 기술 스택

**Language**
* C++

**Reverse Engineering Tool**
* IDA
* x32dbg
* Cheat Engine

<br>

## 👤 담당 역할

**게임 로직 분석**
* 지뢰찾기에서 지뢰 생성 로직 식별
* 피카츄 배구에서 점수 증가 및 바닥 판정 관련 어셈블리 명령어 분석

<br>

## ⚠️ 프로젝트 중 발생한 이슈 및 해결 과정

### 1. 지뢰찾기 리버싱

**문제 상황**

* 지뢰 위치를 결정하는 핵심 함수가 난독화되어 있어 식별이 어려움
* 정적 분석만으로는 실제 실행 흐름 파악에 한계 존재

**해결 방법**

* x32dbg를 활용해 반복 실행 구간의 메모리 값 변화를 추적
* 게임 시작 시점의 메모리 변동을 기준으로 역추적 수행
* 지뢰 배열이 생성되는 시점의 어셈블리 코드 식별

**적용 효과**

* 지뢰 생성 로직을 정확히 파악
* 게임 시작 시 지뢰 위치를 확인할 수 있는 기반 확보

---

### 2. 피카츄 배구 리버싱

**문제 상황**

* 실행 중인 프로세스에 디버거를 부착해 분석해야 하므로 함수 호출 흐름 추적이 어려움
* 게임 상태(점수, 충돌 판정 등)가 어디서 변경되는지 식별 필요

**해결 방법**
* Cheat Engine으로 점수 값 스캔 후 증가 시점의 어셈블리 명령어 탐색
* x32dbg로 해당 코드 영역을 심층 분석
* 바닥 충돌 판정을 수행하는 명령어 구간 식별

**적용 효과**

* 점수 조작 및 게임 로직 변조 가능 지점 확보

---

### 3. PID 수동 입력의 비효율성

**문제 상황**

* 치트 적용 시마다 PID를 직접 확인하고 입력해야 하는 번거로움
* 반복 작업으로 분석 효율 저하

**해결 방법**

* CreateToolhelp32Snapshot()을 활용해 프로세스 목록 수집
* 파일 이름 기반 PID 자동 탐색 로직 구현

```C
DWORD GetProcessID(const WCHAR* processName) {
    HANDLE hSnapshot;
    PROCESSENTRY32 pe32;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // 함수가 호출된 직후에 프로세스 ID, 이름 등 PROCESSENTRY32 구조체에 들어갈 정보를 수집

    pe32.dwSize = sizeof(PROCESSENTRY32);
    // dwSize를 초기화하지 않으면 Process32First가 실패해서 초기화해야 함

    Process32First(hSnapshot, &pe32);
    // 스냅샷으로 생성된 첫번째 정보를 PROCESSENTRY32 구조체에 저장

    do {
        if (wcscmp(pe32.szExeFile, processName) == 0) { // ==로 하면 szExeFile의 주소값이랑 비교하게 되서 wcscmp 사용
            CloseHandle(hSnapshot);
            return pe32.th32ProcessID; // PROCESSENTRY32 구조체 저장된 실행파일 이름과 processName이 동일하면 해당 PID 리턴
        }
    } while (Process32Next(hSnapshot, &pe32)); // 이름이 동일하지 않으면 다음 프로세스 정보를 pe32에 저장

    CloseHandle(hSnapshot);

    return 0;
}
```

**적용 효과**

* 분석 준비 시간 단축
