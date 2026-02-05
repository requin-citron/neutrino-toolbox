#include "neutrino/peb_walking.h"
#include "neutrino/utils.h"
#include "debug.h"

PBYTE ldr_find_module(PPEB_LDR_DATA ldr, PCHAR target_module_name) {
    PLIST_ENTRY linked_lst = &ldr->InLoadOrderModuleList;
    PLIST_ENTRY curr       = linked_lst->Flink;


    while(linked_lst != curr){
        PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(curr, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        _inf("Module Base: 0x%p", entry->DllBase);
        _inf("Module Size: 0x%lx", entry->SizeOfImage);

        WCHAR buffer[MAX_PATH] = {0};
        wcsncpy_s(buffer, entry->BaseDllName.Buffer, entry->BaseDllName.Length / sizeof(WCHAR));
        _inf("Module Name: %ls", buffer);
        _inf("---------------------------");

        if (hash_x65599((PCHAR)neutrino_wchar_to_char(entry->BaseDllName.Buffer), entry->BaseDllName.Length / sizeof(WCHAR)) == hash_x65599(target_module_name, lstrlenA(target_module_name))) {
            _inf("Found %s, resolving functions...", target_module_name);
            return (PBYTE)entry->DllBase;
        }

        curr = curr->Flink;
    }

    return NULL;
}

PHASHMAP init_function_map() {
    PHASHMAP func_map = hashmap_create(PEB_WALK_HASH_MAP_SIZE);

    if (!func_map) {
        _err("Failed to create function hashmap");
        return NULL;
    }

    return func_map;
}

VOID resolv_functions(PHASHMAP func_map, PBYTE dllbaseaddr){

    PIMAGE_DOS_HEADER img_dos_header      = (PIMAGE_DOS_HEADER)dllbaseaddr;

     // Vérifier la signature DOS "MZ"
    if (img_dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        _inf("Invalid DOS signature");
        return;
    }

    PIMAGE_NT_HEADERS img_header          = (PIMAGE_NT_HEADERS)((PBYTE)dllbaseaddr + img_dos_header->e_lfanew);

    // Vérifier la signature PE "PE\0\0"
    if (img_header->Signature != IMAGE_NT_SIGNATURE) {
        _inf("Invalid NT signature");
        return;
    }

    PIMAGE_OPTIONAL_HEADER img_opt_header = &img_header->OptionalHeader;
    PIMAGE_DATA_DIRECTORY img_data_dir    = &img_opt_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    // Vérifier si le module a une table d'export
    if (img_data_dir->VirtualAddress == 0 || img_data_dir->Size == 0) {
        _inf("No export table");
        return;
    }

    PIMAGE_EXPORT_DIRECTORY img_exp_dir   = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)dllbaseaddr + img_data_dir->VirtualAddress);
    PDWORD func_addr                      = (PDWORD)((PBYTE)dllbaseaddr + img_exp_dir->AddressOfFunctions);
    PDWORD func_name_rva                  = (PDWORD)((PBYTE)dllbaseaddr + img_exp_dir->AddressOfNames);
    PWORD  func_ordinal                   = (PWORD)((PBYTE)dllbaseaddr + img_exp_dir->AddressOfNameOrdinals);

    DWORD export_start = img_data_dir->VirtualAddress;
    DWORD export_end   = export_start + img_data_dir->Size;

    for (DWORD i = 0; i < img_exp_dir->NumberOfNames; ++i){
        PCHAR curr_func_name = (PCHAR)((PBYTE)dllbaseaddr + func_name_rva[i]);
        WORD ord             = func_ordinal[i];
        DWORD func_rva       = func_addr[ord];

        if(hashmap_get(func_map, curr_func_name, lstrlenA(curr_func_name)) != NULL){
            continue; // Function already resolved
        }

        // Skip forwarded exports (RVA points inside export directory)
        if (func_rva >= export_start && func_rva < export_end) {
            continue;
        }

        PVOID address = (PVOID)((PBYTE)dllbaseaddr + func_rva);

        hashmap_insert(func_map, curr_func_name, lstrlenA(curr_func_name), address);
    }

    return;
}

BOOL insert_new_dll(PHASHMAP func_map, PCHAR dll_name) {
    if (!func_map || !dll_name) {
        _err("Invalid parameters to insert_new_dll");
        return FALSE;
    }

    PEB* peb          = (PEB*)__readgsqword(0x60);
    PEB_LDR_DATA* ldr = (PEB_LDR_DATA*)peb->Ldr;

    PBYTE dll_base = ldr_find_module(ldr, dll_name);
    if (!dll_base) {

        dll_base = (PBYTE)LoadLibraryA(dll_name);

        if(dll_base == NULL){ // Failed to load the DLL
            _err("Failed to load module: %s", dll_name);
            return FALSE;
        }
    }

    resolv_functions(func_map, dll_base);

    return TRUE;
}
