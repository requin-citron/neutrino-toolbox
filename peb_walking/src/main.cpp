#include "main.h"
#include "xorstr.h"
#include <iphlpapi.h>
#include <lm.h>
#include <lmwksta.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef DWORD (WINAPI* fnGetCurrentProcessId)(VOID);
typedef VOID (WINAPI* fnRtlMoveMemory)(PBYTE, PBYTE, SIZE_T);
typedef DWORD (WINAPI* fnNetWkstaGetInfo)(LPCWSTR, DWORD, LPBYTE*);
typedef INT (WINAPI* fnlstrlenW)(LPCWSTR);
typedef DWORD (WINAPI* fnNetApiBufferFree)(LPVOID);

// Winsock function typedefs
typedef INT (WINAPI* fnWSAStartup)(WORD, LPWSADATA);
typedef INT (WINAPI* fnWSACleanup)(VOID);
typedef SOCKET (WINAPI* fnSocket)(INT, INT, INT);
typedef INT (WINAPI* fnConnect)(SOCKET, const struct sockaddr*, INT);
typedef INT (WINAPI* fnSend)(SOCKET, const char*, INT, INT);
typedef INT (WINAPI* fnRecv)(SOCKET, char*, INT, INT);
typedef INT (WINAPI* fnClosesocket)(SOCKET);
typedef INT (WINAPI* fnGetaddrinfo)(PCSTR, PCSTR, const ADDRINFOA*, PADDRINFOA*);
typedef VOID (WINAPI* fnFreeaddrinfo)(PADDRINFOA);
typedef INT (WINAPI* fnWSAGetLastError)(VOID);

fnNetWkstaGetInfo pNetWkstaGetInfo = NULL;
fnlstrlenW plstrlenW = NULL;
fnNetApiBufferFree pNetApiBufferFree = NULL;
fnRtlMoveMemory pRtlMoveMemory = NULL;

// Winsock function pointers
fnWSAStartup pWSAStartup = NULL;
fnWSACleanup pWSACleanup = NULL;
fnSocket pSocket = NULL;
fnConnect pConnect = NULL;
fnSend pSend = NULL;
fnRecv pRecv = NULL;
fnClosesocket pClosesocket = NULL;
fnGetaddrinfo pGetaddrinfo = NULL;
fnFreeaddrinfo pFreeaddrinfo = NULL;
fnWSAGetLastError pWSAGetLastError = NULL;

extern "C" VOID __main(){
    return;
}

// Getting current domain
PWSTR get_domain(){
    DWORD dwLevel = 102;
	LPWKSTA_INFO_102 pBuf = NULL;
	PWCHAR domain = NULL;
	NET_API_STATUS nStatus;
	LPWSTR pszServerName = NULL;
    _inf("before NetWkstaGetInfo");
	nStatus = pNetWkstaGetInfo(pszServerName, dwLevel, (LPBYTE*)&pBuf);
    _inf("after NetWkstaGetInfo");
	if (nStatus == NERR_Success)
	{
		DWORD length = plstrlenW(pBuf->wki102_langroup);
        _inf("after lstrlenW");
		domain = (PWCHAR)mcalloc(sizeof(WCHAR) * (length + 1));
        pRtlMoveMemory((PBYTE)domain, (PBYTE)pBuf->wki102_langroup, sizeof(WCHAR) * length);
        _inf("after RtlMoveMemory");
	}
	if (pBuf != NULL && pNetApiBufferFree != NULL)
		pNetApiBufferFree(pBuf);
	return domain;
}

// Test Winsock dynamic resolution
BOOL test_winsock(){
    _inf("Starting Winsock test...");

    WSADATA wsaData;
    INT result = pWSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        _err("WSAStartup failed: %d", result);
        return FALSE;
    }
    _inf("WSAStartup successful, version: %d.%d", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));

    // Create a socket
    SOCKET sock = pSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        _err("socket() failed: %d", pWSAGetLastError());
        pWSACleanup();
        return FALSE;
    }
    _inf("Socket created successfully: 0x%llx", (unsigned long long)sock);

    // Test getaddrinfo for google.com
    struct addrinfo hints = {0}, *result_addr = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    INT ret = pGetaddrinfo("www.google.com", "80", &hints, &result_addr);
    if (ret != 0) {
        _err("getaddrinfo() failed: %d", ret);
        pClosesocket(sock);
        pWSACleanup();
        return FALSE;
    }
    _inf("getaddrinfo() successful for www.google.com");

    // Try to connect
    ret = pConnect(sock, result_addr->ai_addr, (int)result_addr->ai_addrlen);
    if (ret == SOCKET_ERROR) {
        _inf("connect() failed (expected in most environments): %d", pWSAGetLastError());
    } else {
        _inf("connect() successful!");

        // Send a simple HTTP request
        const char* http_request = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
        INT sent = pSend(sock, http_request, lstrlenA(http_request), 0);
        if (sent > 0) {
            _inf("Sent %d bytes", sent);

            // Try to receive response - use dynamic allocation to avoid stack overflow
            char* recv_buffer = (char*)mcalloc(256);
            if (recv_buffer) {
                INT received = pRecv(sock, recv_buffer, 255, 0);
                if (received > 0) {
                    _inf("Received %d bytes: %.80s...", received, recv_buffer);
                } else if (received == 0) {
                    _inf("Connection closed by remote host");
                } else {
                    _inf("recv() failed: %d", pWSAGetLastError());
                }
                mcfree(recv_buffer);
            } else {
                _err("Failed to allocate receive buffer");
            }
        }
    }

    // Cleanup
    if (result_addr)
        pFreeaddrinfo(result_addr);
    pClosesocket(sock);
    pWSACleanup();
    _inf("Winsock cleanup complete");

    return TRUE;
}

INT main(){

    _inf("Starting PEB walking demo...");
    _inf("test peb resolution: %p", get_peb_stealth());

    PHASHMAP func_map = init_function_map();

    // hashmap_insert(func_map, (PVOID)xorstr_("NetWkstaGetInfo"), 16, (PVOID)0xdeadbeef); // Test insert and get
    // PVOID toto = hashmap_get(func_map, (PVOID)xorstr_("NetWkstaGetInfo"), 16); // Test get existing key


    insert_new_dll(func_map, xorstr_("NTDLL.DLL"));
    _inf("---------------------------");
    insert_new_dll(func_map, xorstr_("KERNELBASE.DLL"));
    _inf("---------------------------");
    insert_new_dll(func_map, xorstr_("KERNEL32.DLL"));
    insert_new_dll(func_map, xorstr_("NETUTILS.DLL"));
    insert_new_dll(func_map, xorstr_("IPHLPAPI.DLL"));
    insert_new_dll(func_map, xorstr_("NETAPI32.DLL"));
    _inf("---------------------------");
    insert_new_dll(func_map, xorstr_("WS2_32.DLL")); // Winsock DLL
    
#if _DEBUG == 1
    // hashmap_print(func_map);
#endif

    _inf("---------------------------");
    _inf("hashmap count: %lu", func_map->count);

    pNetWkstaGetInfo = (fnNetWkstaGetInfo)hashmap_get(func_map, (PVOID)xorstr_("NetWkstaGetInfo"), lstrlenA(xorstr_("NetWkstaGetInfo")));
    plstrlenW = (fnlstrlenW)hashmap_get(func_map, (PVOID)xorstr_("lstrlenW"), lstrlenA(xorstr_("lstrlenW")));
    pNetApiBufferFree = (fnNetApiBufferFree)hashmap_get(func_map, (PVOID)xorstr_("NetApiBufferFree"), lstrlenA(xorstr_("NetApiBufferFree")));
    pRtlMoveMemory = (fnRtlMoveMemory)hashmap_get(func_map, (PVOID)xorstr_("RtlMoveMemory"), lstrlenA(xorstr_("RtlMoveMemory")));

    _inf("pNetApiBufferFree resovled function %p", pNetApiBufferFree);
    _inf("plstrlenW resovled function %p", plstrlenW);
    _inf("pNetWkstaGetInfo resovled function %p", pNetWkstaGetInfo);
    _inf("toto %ls", get_domain());

    _inf("---------------------------");
    _inf("Resolving Winsock functions...");

    // Resolve Winsock functions
    pWSAStartup = (fnWSAStartup)hashmap_get(func_map, (PVOID)xorstr_("WSAStartup"), lstrlenA(xorstr_("WSAStartup")));
    pWSACleanup = (fnWSACleanup)hashmap_get(func_map, (PVOID)xorstr_("WSACleanup"), lstrlenA(xorstr_("WSACleanup")));
    pSocket = (fnSocket)hashmap_get(func_map, (PVOID)xorstr_("socket"), lstrlenA(xorstr_("socket")));
    pConnect = (fnConnect)hashmap_get(func_map, (PVOID)xorstr_("connect"), lstrlenA(xorstr_("connect")));
    pSend = (fnSend)hashmap_get(func_map, (PVOID)xorstr_("send"), lstrlenA(xorstr_("send")));
    pRecv = (fnRecv)hashmap_get(func_map, (PVOID)xorstr_("recv"), lstrlenA(xorstr_("recv")));
    pClosesocket = (fnClosesocket)hashmap_get(func_map, (PVOID)xorstr_("closesocket"), lstrlenA(xorstr_("closesocket")));
    pGetaddrinfo = (fnGetaddrinfo)hashmap_get(func_map, (PVOID)xorstr_("getaddrinfo"), lstrlenA(xorstr_("getaddrinfo")));
    pFreeaddrinfo = (fnFreeaddrinfo)hashmap_get(func_map, (PVOID)xorstr_("freeaddrinfo"), lstrlenA(xorstr_("freeaddrinfo")));
    pWSAGetLastError = (fnWSAGetLastError)hashmap_get(func_map, (PVOID)xorstr_("WSAGetLastError"), lstrlenA(xorstr_("WSAGetLastError")));

    _inf("pWSAStartup resolved: %p", pWSAStartup);
    _inf("pWSACleanup resolved: %p", pWSACleanup);
    _inf("pSocket resolved: %p", pSocket);
    _inf("pConnect resolved: %p", pConnect);
    _inf("pSend resolved: %p", pSend);
    _inf("pRecv resolved: %p", pRecv);
    _inf("pClosesocket resolved: %p", pClosesocket);
    _inf("pGetaddrinfo resolved: %p", pGetaddrinfo);
    _inf("pFreeaddrinfo resolved: %p", pFreeaddrinfo);
    _inf("pWSAGetLastError resolved: %p", pWSAGetLastError);

    _inf("---------------------------");

    // Test Winsock if all functions are resolved
    if (pWSAStartup && pWSACleanup && pSocket && pConnect &&
        pSend && pRecv && pClosesocket && pGetaddrinfo &&
        pFreeaddrinfo && pWSAGetLastError) {
        test_winsock();
    } else {
        _err("Failed to resolve all Winsock functions!");
    }

    // parse_linklist_inloadorder_module(&ldr->InLoadOrderModuleList);
    // parse_linklist_hashlinks_module(&ldr->InLoadOrderModuleList);

    return 0;
}