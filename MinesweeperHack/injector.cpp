#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include "TCHAR.h"
#include "injector.h"

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    TOKEN_PRIVILEGES tp;
    HANDLE hToken;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
        &hToken))
    {
        _tprintf(L"OpenProcessToken error: %u\n", GetLastError());
        return FALSE;
    }

    if (!LookupPrivilegeValue(NULL,
        lpszPrivilege,
        &luid))
    {
        _tprintf(L"LookupPrivilegeValue error: %u\n", GetLastError());
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    if (!AdjustTokenPrivileges(hToken,
        FALSE,
        &tp,
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES)NULL,
        (PDWORD)NULL))
    {
        _tprintf(L"AdjustTokenPrivileges error: %u\n", GetLastError());
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        _tprintf(L"The token does not have the specified privilege. \n");
        return FALSE;
    }

    return TRUE;
}


DWORD GetProcessID(const WCHAR* processName) {
    HANDLE hSnapshot;
    PROCESSENTRY32 pe32;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    pe32.dwSize = sizeof(PROCESSENTRY32);

    Process32First(hSnapshot, &pe32);

    do {
        if (wcscmp(pe32.szExeFile, processName) == 0) {
            CloseHandle(hSnapshot);
            return pe32.th32ProcessID;
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);

    return 0;
}


BOOL InjectDLL(DWORD PID, LPCTSTR dllPath) {

    HANDLE hProcess = NULL, hThread = NULL;
    HMODULE hKernel = NULL;
    LPVOID VMAddress = NULL;
    SIZE_T dllSize = (wcslen(dllPath) + 1) * sizeof(TCHAR);
    FARPROC LoadLibraryAddr;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (hProcess == NULL) { printf("OpenProcess 실패\n"); }

    VMAddress = VirtualAllocEx(hProcess, NULL, dllSize, MEM_COMMIT, PAGE_READWRITE);
    if (VMAddress == NULL) { printf("VirtualAllocEx 실패\n"); }

    WriteProcessMemory(hProcess, VMAddress, (LPVOID)dllPath, dllSize, NULL);

    hKernel = GetModuleHandle(L"Kernel32.dll");
    if (hKernel == NULL) { printf("GetModuleHandle 실패\n"); }

    LoadLibraryAddr = GetProcAddress(hKernel, "LoadLibraryW");
    if (LoadLibraryAddr == NULL) { printf("GetProcAddress 실패\n"); }

    hThread = CreateRemoteThread(hProcess, NULL, dllSize, (LPTHREAD_START_ROUTINE)LoadLibraryAddr, VMAddress, 1, NULL);
    if (hThread == NULL) { printf("CreateRemoteThread 실패\n"); }

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}