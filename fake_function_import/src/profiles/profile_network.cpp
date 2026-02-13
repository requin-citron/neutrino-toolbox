#include "../main.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winhttp.h>
#include <wincrypt.h>
#include <bcrypt.h>
#include <windns.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "dnsapi.lib")

__attribute__((__annotate__(("mba,substitution"))))
VOID execute_network_profile() {
    WSADATA wsaData;
    WORD versionRequested = MAKEWORD(2, 2);
    int wsaResult = WSAStartup(versionRequested, &wsaData);
    if (wsaResult == 0) {
        WSACleanup();
    }

    HCRYPTPROV hProv = 0;
    DWORD provType = (0x1);
    DWORD flags = (0xF0000000);
    if (CryptAcquireContextW(&hProv, NULL, NULL, provType, flags)) {
        BYTE randBuffer[16];
        CryptGenRandom(hProv, sizeof(randBuffer), randBuffer);
        CryptReleaseContext(hProv, 0);
    }

    DWORD accessType = (0x0);
    HINTERNET hSession = WinHttpOpen(L"NetworkClient/1.0", accessType, NULL, NULL, 0);
    if (hSession) {
        DWORD optionValue = 0;
        DWORD optionSize = sizeof(optionValue);
        DWORD timeoutOption = (0x2);

        WinHttpQueryOption(hSession, timeoutOption, &optionValue, &optionSize);

        DWORD newTimeout = (0x7530);
        WinHttpSetOption(hSession, timeoutOption, &newTimeout, sizeof(newTimeout));

        WinHttpCloseHandle(hSession);
    }

    DWORD encodingType = (0x1 | 0x10000);
    DWORD storeFlags = (0x10000);
    HCERTSTORE hCertStore = CertOpenStore((LPCSTR)10, encodingType, 0, storeFlags, L"MY");
    if (hCertStore) {
        CertCloseStore(hCertStore, 0);
    }

    BCRYPT_ALG_HANDLE hAlgorithm = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlgorithm, L"AES", NULL, 0);
    if (status == 0) {
        BYTE bcryptRand[32];
        ULONG randFlags = (0x2);
        BCryptGenRandom(NULL, bcryptRand, sizeof(bcryptRand), randFlags);

        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
    }

    if (WSAStartup(versionRequested, &wsaData) == 0) {
        int af = (0x2);
        int type = (0x1);
        int proto = (0x6);

        SOCKET sock = socket(af, type, proto);
        if (sock != INVALID_SOCKET) {
            bind(sock, NULL, 0);
            closesocket(sock);
        }

        WSACleanup();
    }
}
