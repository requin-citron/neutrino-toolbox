#include "config.h"
#include <iphlpapi.h>
#include <lm.h>
#include <lmwksta.h>

typedef DWORD (WINAPI* fnGetCurrentProcessId)(VOID);
typedef VOID (WINAPI* fnRtlMoveMemory)(PBYTE, PBYTE, SIZE_T);
typedef DWORD (WINAPI* fnNetWkstaGetInfo)(LPCWSTR, DWORD, LPBYTE*);
typedef INT (WINAPI* fnlstrlenW)(LPCWSTR);
typedef DWORD (WINAPI* fnNetApiBufferFree)(LPVOID);


fnNetWkstaGetInfo pNetWkstaGetInfo = NULL;
fnlstrlenW plstrlenW = NULL;
fnNetApiBufferFree pNetApiBufferFree = NULL;
fnRtlMoveMemory pRtlMoveMomory = NULL;

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
        pRtlMoveMomory((PBYTE)domain, (PBYTE)pBuf->wki102_langroup, sizeof(WCHAR) * length);
        _inf("after RtlMoveMemory");
	}
	if (pBuf != NULL && pNetApiBufferFree != NULL)
		pNetApiBufferFree(pBuf);
	return domain;
}

INT main(){

    PEB* peb          = (PEB*)__readgsqword(0x60);
    PEB_LDR_DATA* ldr = (PEB_LDR_DATA*)peb->Ldr;

    HANDLE kern32     = LoadLibraryA("kernel32.dll");

    _inf("kernel32.dll loaded at address: 0x%p", kern32);
    _inf("hash_x65599(\"kernel32.dll\") = 0x%x", hash_x65599((PCHAR)"kernel32.dll", sizeof("kernel32.dll") - 1));

    _inf("PEB address: 0x%p", peb);
    _inf("Ldr address: 0x%p", ldr);
    _inf("---------------------------");

    PHASHMAP func_map = init_function_map();
    insert_new_dll(func_map, "ntdll.dll");
    insert_new_dll(func_map, "kernel32.dll");
    insert_new_dll(func_map, "ADVAPI32.dll");
    insert_new_dll(func_map, "IPHLPAPI.dll");
    insert_new_dll(func_map, "NETAPI32.dll");

#if _DEBUG == 1
    // hashmap_print(func_map);
#endif

    _inf("---------------------------");
    _inf("hashmap count: %lu", func_map->count);
    CHAR test[]  = "GetCurrentProcessId";

    fnGetCurrentProcessId pGetCurrentProcessId = (fnGetCurrentProcessId)hashmap_get(func_map, test, lstrlenA(test));
    
    // pRtlMoveMomory = (fnRtlMoveMemory)hashmap_get(func_map, test2, lstrlenA(test2));
    // pNetWkstaGetInfo = (fnNetWkstaGetInfo)hashmap_get(func_map, (PVOID)"NetWkstaGetInfo", lstrlenA("NetWkstaGetInfo"));
    // plstrlenW = (fnlstrlenW)hashmap_get(func_map, (PVOID)"lstrlenW", lstrlenA("lstrlenW"));
    // pNetApiBufferFree = (fnNetApiBufferFree)hashmap_get(func_map, (PVOID)"NetApiBufferFree", lstrlenA("NetApiBufferFree"));

    // _inf("resovled function %p\n", pNetApiBufferFree);

    // if (pGetCurrentProcessId) {
    //     DWORD pid = pGetCurrentProcessId();
    //     _inf("Test call success! PID = %lu", pid);
    // }

    // _inf("toto %ls", get_domain());

    // parse_linklist_inloadorder_module(&ldr->InLoadOrderModuleList);
    // parse_linklist_hashlinks_module(&ldr->InLoadOrderModuleList);

    return 0;
}