#include "config.h"

typedef DWORD (WINAPI* fnGetCurrentProcessId)(VOID);
typedef VOID (WINAPI* fnRtlMoveMemory)(PBYTE, PBYTE, SIZE_T);

extern "C" VOID __main(){
    return;
}

// VOID resolv_all_functions(PBYTE dllbaseaddr){

//     PIMAGE_DOS_HEADER img_dos_header      = (PIMAGE_DOS_HEADER)dllbaseaddr;

//      // Vérifier la signature DOS "MZ"
//     if (img_dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
//         _inf("Invalid DOS signature");
//         return;
//     }

//     PIMAGE_NT_HEADERS img_header          = (PIMAGE_NT_HEADERS)((PBYTE)dllbaseaddr + img_dos_header->e_lfanew);
    
//     // Vérifier la signature PE "PE\0\0"
//     if (img_header->Signature != IMAGE_NT_SIGNATURE) {
//         _inf("Invalid NT signature");
//         return;
//     }
    
//     PIMAGE_OPTIONAL_HEADER img_opt_header = &img_header->OptionalHeader; 
//     PIMAGE_DATA_DIRECTORY img_data_dir    = &img_opt_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    
//     // Vérifier si le module a une table d'export
//     if (img_data_dir->VirtualAddress == 0 || img_data_dir->Size == 0) {
//         _inf("No export table");
//         return;
//     }
    
//     PIMAGE_EXPORT_DIRECTORY img_exp_dir   = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)dllbaseaddr + img_data_dir->VirtualAddress);
//     PDWORD func_addr                      = (PDWORD)((PBYTE)dllbaseaddr + img_exp_dir->AddressOfFunctions);
//     PDWORD func_name_rva                  = (PDWORD)((PBYTE)dllbaseaddr + img_exp_dir->AddressOfNames);
//     PWORD  func_ordinal                   = (PWORD)((PBYTE)dllbaseaddr + img_exp_dir->AddressOfNameOrdinals);

//     _inf("---------------------------");

//     for (DWORD i = 0; i < img_exp_dir->NumberOfNames; ++i){
//         PCHAR curr_func_name = (PCHAR)((PBYTE)dllbaseaddr + func_name_rva[i]);
//         WORD ord             = func_ordinal[i];
//         PVOID address        = (PVOID)((PBYTE)dllbaseaddr + func_addr[ord]);

//         _inf("Resolved %s at address: 0x%p", curr_func_name, address);
//         // if (strcmp(curr_func_name, func_name) == 0){
//         //     WORD ord = func_ordinal[i];
//         //     PVOID address = (PVOID)((PBYTE)dllbaseaddr + func_addr[ord]);
//         //     _inf("Resolved %s at address: 0x%p", func_name, address);
//         //     return;
//         // }
//     }

//     _inf("---------------------------");

//     return;
// }

// VOID parse_linklist_inloadorder_module(PLIST_ENTRY linked_lst){
//     PLIST_ENTRY curr  = linked_lst->Flink;
    

//     while(linked_lst != curr){
//         PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(curr, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
        
//         _inf("Module Base: 0x%p", entry->DllBase);
//         _inf("Module Size: 0x%lx", entry->SizeOfImage);
        
//         WCHAR buffer[MAX_PATH] = {0};
//         wcsncpy_s(buffer, entry->BaseDllName.Buffer, entry->BaseDllName.Length / sizeof(WCHAR));
//         _inf("Module Name: %ws", buffer);
//         _inf("---------------------------");

//         if (hash_x65599((PCHAR)neutrino_wchar_to_char(entry->BaseDllName.Buffer), entry->BaseDllName.Length / sizeof(WCHAR)) == 0x6A4ABC5B) {
//             _inf("Found kernel32.dll, resolving functions...");
//             resolv_functions((PBYTE)entry->DllBase);
//         }

//         curr = curr->Flink;
//     }

//     return;
// }

// VOID parse_linklist_hashlinks_module(PLIST_ENTRY linked_lst){
//     PLIST_ENTRY curr  = linked_lst->Flink;
    

//     while(linked_lst != curr){
//         PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(curr, LDR_DATA_TABLE_ENTRY, HashLinks);
        
//         _inf("Module Base: 0x%p", entry->DllBase);
//         _inf("Module Size: 0x%lx", entry->SizeOfImage);
//         _inf("Module Hash: 0x%p", entry->HashLinks);
//         _inf("---------------------------");

//         curr = curr->Flink;
//     }

//     return;
// }

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

#if _DEBUG == 1
    // hashmap_print(func_map);
#endif

    _inf("---------------------------");
    _inf("hashmap count: %lu", func_map->count);
    CHAR test[]  = "GetCurrentProcessId";
    CHAR test2[] = "RtlMoveMemory";

    fnGetCurrentProcessId pGetCurrentProcessId = (fnGetCurrentProcessId)hashmap_get(func_map, test, lstrlenA(test));
    fnRtlMoveMemory pRtlMoveMomory = (fnRtlMoveMemory)hashmap_get(func_map, test2, lstrlenA(test2));

    _inf("resovled function %p\n", pRtlMoveMomory);

    if (pGetCurrentProcessId) {
        DWORD pid = pGetCurrentProcessId();
        _inf("Test call success! PID = %lu", pid);
    }

    pRtlMoveMomory((PBYTE)test2, (PBYTE)"toto\0", 5);
    _inf("debug %s", test2);

    // parse_linklist_inloadorder_module(&ldr->InLoadOrderModuleList);
    // parse_linklist_hashlinks_module(&ldr->InLoadOrderModuleList);

    return 0;
}