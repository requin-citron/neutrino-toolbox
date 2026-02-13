#include "../main.h"
#include <psapi.h>
#include <wtsapi32.h>
#include <userenv.h>
#include <lm.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "netapi32.lib")

__attribute__((__annotate__(("bcf,substitution"))))
VOID execute_system_profile() {
    HKEY hKey = NULL;
    DWORD keyAccess = (0x20019);
    LSTATUS regStatus = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                                       L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                                       0, keyAccess, &hKey);
    if (regStatus == ERROR_SUCCESS) {
        WCHAR productName[256];
        DWORD dataSize = sizeof(productName);
        DWORD dataType = 0;

        RegQueryValueExW(hKey, L"ProductName", NULL, &dataType, (LPBYTE)productName, &dataSize);

        WCHAR keyName[256];
        DWORD keyNameSize = sizeof(keyName) / sizeof(WCHAR);
        DWORD index = 0;
        RegEnumKeyExW(hKey, index, keyName, &keyNameSize, NULL, NULL, NULL, NULL);

        WCHAR valueName[256];
        DWORD valueNameSize = sizeof(valueName) / sizeof(WCHAR);
        RegEnumValueW(hKey, index, valueName, &valueNameSize, NULL, NULL, NULL, NULL);

        RegCloseKey(hKey);
    }

    DWORD processIds[1024];
    DWORD bytesReturned = 0;
    if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
        DWORD currentPid = GetCurrentProcessId();
        DWORD processAccess = (0x1000 | 0x0400);

        HANDLE hProcess = OpenProcess(processAccess, FALSE, currentPid);
        if (hProcess) {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            }

            WCHAR exePath[MAX_PATH];
            GetModuleFileNameExW(hProcess, NULL, exePath, MAX_PATH);

            HMODULE hModules[256];
            DWORD modulesSize = 0;
            if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &modulesSize)) {
                if (modulesSize > 0) {
                    MODULEINFO modInfo;
                    GetModuleInformation(hProcess, hModules[0], &modInfo, sizeof(modInfo));
                }
            }

            CloseHandle(hProcess);
        }
    }

    PERFORMANCE_INFORMATION perfInfo;
    GetPerformanceInfo(&perfInfo, sizeof(perfInfo));

    DWORD scmAccess = (0x0001 | 0x0004);
    SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, scmAccess);
    if (hSCM) {
        DWORD bytesNeeded = 0;
        DWORD servicesReturned = 0;
        DWORD resumeHandle = 0;
        DWORD serviceType = (0x30);
        DWORD serviceState = (0x3);

        EnumServicesStatusW(hSCM, serviceType, serviceState, NULL, 0,
                            &bytesNeeded, &servicesReturned, &resumeHandle);

        DWORD serviceAccess = (0x0004);
        SC_HANDLE hService = OpenServiceW(hSCM, L"EventLog", serviceAccess);
        if (hService) {
            SERVICE_STATUS serviceStatus;
            QueryServiceStatus(hService, &serviceStatus);

            BYTE statusBuffer[1024];
            DWORD statusBytesNeeded = 0;
            QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, statusBuffer,
                                 sizeof(statusBuffer), &statusBytesNeeded);

            CloseServiceHandle(hService);
        }

        CloseServiceHandle(hSCM);
    }

    HANDLE hEventLog = OpenEventLogW(NULL, L"System");
    if (hEventLog) {
        BYTE eventBuffer[1024];
        DWORD bytesRead = 0;
        DWORD minBytesNeeded = 0;
        DWORD readFlags = (0x0001 | 0x0004);

        ReadEventLogW(hEventLog, readFlags, 0, eventBuffer, sizeof(eventBuffer),
                      &bytesRead, &minBytesNeeded);

        CloseEventLog(hEventLog);
    }

    PWTS_SESSION_INFOW pSessionInfo = NULL;
    DWORD sessionCount = 0;
    if (WTSEnumerateSessionsW(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &sessionCount)) {
        if (sessionCount > 0 && pSessionInfo) {
            LPWSTR pBuffer = NULL;
            DWORD bytesReturned = 0;
            WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE,
                                         pSessionInfo[0].SessionId,
                                         WTSUserName,
                                         &pBuffer,
                                         &bytesReturned);
            if (pBuffer) {
                WTSFreeMemory(pBuffer);
            }
        }

        WTSFreeMemory(pSessionInfo);
    }

    WCHAR profilesDir[MAX_PATH];
    DWORD profilesDirSize = MAX_PATH;
    GetProfilesDirectoryW(profilesDir, &profilesDirSize);

    LPBYTE pWkstaInfo = NULL;
    NET_API_STATUS netStatus = NetWkstaGetInfo(NULL, 100, &pWkstaInfo);
    if (netStatus == NERR_Success && pWkstaInfo) {
        NetApiBufferFree(pWkstaInfo);
    }
}
