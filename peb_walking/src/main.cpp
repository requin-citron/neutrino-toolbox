#include "config.h"
#include "xorstr.h"
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
fnRtlMoveMemory pRtlMoveMemory = NULL;

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

INT main(){
    PHASHMAP func_map = init_function_map();

    insert_new_dll(func_map, xorstr_("ntdll.dll"));
    insert_new_dll(func_map, xorstr_("kernel32.dll"));
    insert_new_dll(func_map, xorstr_("NETUTILS.dll"));
    insert_new_dll(func_map, xorstr_("IPHLPAPI.dll"));
    insert_new_dll(func_map, xorstr_("NETAPI32.dll"));
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

    // parse_linklist_inloadorder_module(&ldr->InLoadOrderModuleList);
    // parse_linklist_hashlinks_module(&ldr->InLoadOrderModuleList);

    return 0;
}