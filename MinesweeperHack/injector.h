#pragma once
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
DWORD GetProcessID(const WCHAR* processName);
BOOL InjectDLL(DWORD PID, LPCTSTR dllPath);