/**
 * @file profile_explorer.cpp
 * @brief PROFILE_EXPLORER - Modern file browser with cloud sync/update checker
 *
 * Story: File explorer with cloud synchronization and update verification
 * DLLs: kernel32.dll, shell32.dll, shlwapi.dll, user32.dll, winhttp.dll
 * ~25-28 static imports for credible IAT
 *
 * Ultra-banal profile: File enumeration + HTTP API = OneDrive/Dropbox/Google Drive pattern
 * Zero EDR suspicion: All read-only operations, no crypto, no signatures
 */

#include "../main.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <winhttp.h>

// ============================================================================
// Linker directives - Link required DLLs for file browser profile
// ============================================================================
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "winhttp.lib")

// ============================================================================
// Execution logic - EXEC_FULL strategy with safe, read-only operations
// ============================================================================

/**
 * @brief Execute explorer profile fake imports
 *
 * Categories executed:
 * 1. File enumeration (ultra-banal, used by 99% of applications)
 * 2. File attributes queries (read-only, zero side-effects)
 * 3. Disk information queries (safe system queries)
 * 4. Shell path retrieval (standard system paths)
 * 5. Path utilities (safe path operations)
 * 6. System metrics (UI layout queries)
 * 7. HTTP API checking (cloud sync/update simulation - modern pattern)
 *
 * All operations are read-only and safe. Pattern matches OneDrive/Dropbox/Google Drive.
 * Obfuscated with Polaris annotations (bcf + substitution)
 */
__attribute__((__annotate__(("bcf,substitution"))))
VOID execute_explorer_profile() {
    // ========================================================================
    // Category 1: File enumeration (ultra-banal)
    // ========================================================================

    // Enumerate DLLs in System32 (very common operation)
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(L"C:\\Windows\\System32\\*.dll", &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        // Read a few entries (simulation of file browser loading)
        FindNextFileW(hFind, &findData);
        FindNextFileW(hFind, &findData);
        FindClose(hFind);
    }

    // ========================================================================
    // Category 2: File attributes queries (read-only, safe)
    // ========================================================================

    // Get file attributes for a known system file
    DWORD attrs = GetFileAttributesW(L"C:\\Windows\\System32\\kernel32.dll");

    // Get extended file attributes (size, timestamps)
    WIN32_FILE_ATTRIBUTE_DATA fileAttrData;
    GetFileAttributesExW(L"C:\\Windows\\System32\\kernel32.dll",
                          GetFileExInfoStandard, &fileAttrData);

    // Get file size via handle (safe read-only operation)
    HANDLE hFile = CreateFileW(L"C:\\Windows\\System32\\kernel32.dll",
                                GENERIC_READ, FILE_SHARE_READ,
                                NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        GetFileSizeEx(hFile, &fileSize);

        // Get file timestamps
        FILETIME creationTime, lastAccessTime, lastWriteTime;
        GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime);

        CloseHandle(hFile);
    }

    // ========================================================================
    // Category 3: Disk information queries (safe system queries)
    // ========================================================================

    // Get available logical drives
    DWORD drives = GetLogicalDrives();

    // Get drive type for C: (safe query)
    UINT driveType = GetDriveTypeW(L"C:\\");

    // Get disk free space (read-only query)
    ULARGE_INTEGER freeBytesAvail, totalBytes, freeBytes;
    GetDiskFreeSpaceExW(L"C:\\", &freeBytesAvail, &totalBytes, &freeBytes);

    // Get volume information (safe query)
    WCHAR volumeName[MAX_PATH];
    WCHAR fileSystemName[MAX_PATH];
    DWORD serialNumber, maxComponentLen, fileSystemFlags;
    GetVolumeInformationW(L"C:\\", volumeName, MAX_PATH, &serialNumber,
                           &maxComponentLen, &fileSystemFlags,
                           fileSystemName, MAX_PATH);

    // ========================================================================
    // Category 4: Shell path retrieval (standard system paths)
    // ========================================================================

    // Get Desktop path
    WCHAR desktopPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);

    // Get Documents path
    WCHAR documentsPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documentsPath);

    // Get Program Files path
    WCHAR programFilesPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_PROGRAM_FILES, NULL, 0, programFilesPath);

    // Get file info (safe query)
    SHFILEINFOW fileInfo;
    SHGetFileInfoW(L"C:\\Windows", 0, &fileInfo, sizeof(fileInfo),
                    SHGFI_ICON | SHGFI_DISPLAYNAME);

    // ========================================================================
    // Category 5: Path utilities (safe path operations)
    // ========================================================================

    // Check if path exists
    BOOL exists = PathFileExistsW(L"C:\\Windows");

    // Check if path is a directory
    BOOL isDir = PathIsDirectoryW(L"C:\\Windows");

    // Combine paths (safe string operation)
    WCHAR combined[MAX_PATH];
    PathCombineW(combined, L"C:\\Windows", L"System32");

    // ========================================================================
    // Category 6: System metrics (UI layout queries)
    // ========================================================================

    // Get screen dimensions for UI layout
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int workAreaWidth = GetSystemMetrics(SM_CXFULLSCREEN);
    int workAreaHeight = GetSystemMetrics(SM_CYFULLSCREEN);

    // ========================================================================
    // Category 7: HTTP API check (cloud sync/update simulation)
    // ========================================================================

    // Open HTTP session (user agent: file manager)
    HINTERNET hSession = WinHttpOpen(L"FileExplorer/1.0",
                                       WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                       WINHTTP_NO_PROXY_NAME,
                                       WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession) {
        // Connect to server (simulation of update check)
        HINTERNET hConnect = WinHttpConnect(hSession, L"api.github.com",
                                             INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (hConnect) {
            // Open GET request (safe, read-only)
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
                                                     L"/repos/microsoft/terminal",
                                                     NULL, WINHTTP_NO_REFERER,
                                                     WINHTTP_DEFAULT_ACCEPT_TYPES,
                                                     WINHTTP_FLAG_SECURE);
            if (hRequest) {
                // Send request (may fail due to timeout/network - OK)
                BOOL sent = WinHttpSendRequest(hRequest,
                                                WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                                WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

                if (sent) {
                    // Receive response (safe, may timeout)
                    WinHttpReceiveResponse(hRequest, NULL);

                    // Query HTTP headers (safe)
                    DWORD statusCode = 0;
                    DWORD statusCodeSize = sizeof(statusCode);
                    WinHttpQueryHeaders(hRequest,
                                         WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                         WINHTTP_HEADER_NAME_BY_INDEX,
                                         &statusCode, &statusCodeSize,
                                         WINHTTP_NO_HEADER_INDEX);

                    // Read response data (safe, limited buffer)
                    BYTE buffer[256];
                    DWORD bytesRead = 0;
                    WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead);
                }

                WinHttpCloseHandle(hRequest);
            }
            WinHttpCloseHandle(hConnect);
        }
        WinHttpCloseHandle(hSession);
    }
}
