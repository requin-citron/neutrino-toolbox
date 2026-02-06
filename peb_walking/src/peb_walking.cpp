#include "neutrino/peb_walking.h"

typedef HANDLE     (WINAPI *fn_LoadLibraryA)(PCHAR);

// Validate TEB pointer
__attribute__((__annotate__(("substitution"))))
static BOOL is_valid_teb(PTEB teb) {
    if (!teb) return FALSE;
    if ((ULONG_PTR)teb < 0x1000) return FALSE;
    if ((ULONG_PTR)teb > 0x7FFFFFFFFFFF) return FALSE;

    // Check if PEB pointer looks valid
    PPEB peb = teb->ProcessEnvironmentBlock;
    if (!peb) return FALSE;
    if ((ULONG_PTR)peb < 0x1000) return FALSE;
    if ((ULONG_PTR)peb > 0x7FFFFFFFFFFF) return FALSE;

    // Check if TEB.NtTib.Self points to itself
    if (teb->NtTib.Self != (PNT_TIB)teb) return FALSE;

    return TRUE;
}

// Method 1: TEB self-reference (Primary - Fast & Stealthy)
// Uses gs:0x30 instead of gs:0x60 to avoid detection
__attribute__((__annotate__(("substitution"))))
PTEB get_teb_self_reference() {
    // Calculate offset 0x30 at runtime to avoid signatures
    DWORD64 teb_self_offset = (0x18 << 1) | 0x10;  // = 0x30

    // Read TEB pointer from gs:0x30 (TEB.NtTib.Self)
    PTEB teb = (PTEB)__readgsqword(teb_self_offset);

    if (is_valid_teb(teb)) {
        return teb;
    }

    return NULL;
}

// Main function: Cascading fallback for maximum reliability
__attribute__((__annotate__(("substitution,indirectcall"))))
PPEB get_peb_stealth() {
    PTEB teb = NULL;

    // Try Method 1: Self-reference (fast, stealthy)
    teb = get_teb_self_reference();
    if (teb && teb->ProcessEnvironmentBlock) {
        return teb->ProcessEnvironmentBlock;
    }

    // TODO - Other Method

    // All methods failed
    return NULL;
}

__attribute__((__annotate__(("substitution,indirectcall"))))
PBYTE ldr_find_module(PPEB_LDR_DATA ldr, PCHAR target_module_name) {
    PLIST_ENTRY linked_lst = &ldr->InLoadOrderModuleList;
    PLIST_ENTRY curr       = linked_lst->Flink;


    while(linked_lst != curr){
        PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(curr, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        WCHAR buffer[MAX_PATH] = {0};
        RtlCopyMemory(buffer, entry->BaseDllName.Buffer, entry->BaseDllName.Length / sizeof(WCHAR));

        if (hash_x65599((PCHAR)neutrino_wchar_to_char(entry->BaseDllName.Buffer), entry->BaseDllName.Length / sizeof(WCHAR)) == hash_x65599(target_module_name, lstrlenA(target_module_name))) {
            _inf("Found %s, resolving functions...", target_module_name);
            return (PBYTE)entry->DllBase;
        }

        curr = curr->Flink;
    }

    return NULL;
}

__attribute__((__annotate__(("substitution"))))
PHASHMAP init_function_map() {
    PHASHMAP func_map = hashmap_create(PEB_WALK_HASH_MAP_SIZE);

    if (!func_map) {
        _err("Failed to create function hashmap");
        return NULL;
    }

    return func_map;
}

__attribute__((__annotate__(("substitution,indirectcall"))))
VOID resolv_functions(PHASHMAP func_map, PBYTE dllbaseaddr){

    // Obfuscate offset 0x3C (e_lfanew) - breaks CAPA detection
    DWORD lfanew_offset = (0x1E << 1);  // 0x3C calculated at runtime
    DWORD pe_offset = *(DWORD*)(dllbaseaddr + lfanew_offset);

    PIMAGE_DOS_HEADER img_dos_header = (PIMAGE_DOS_HEADER)dllbaseaddr;

    // Vérifier la signature DOS "MZ"
    if (img_dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        _inf("Invalid DOS signature");
        return;
    }

    PIMAGE_NT_HEADERS img_header = (PIMAGE_NT_HEADERS)(dllbaseaddr + pe_offset);

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

    PBYTE exp_dir_base = (PBYTE)dllbaseaddr + img_data_dir->VirtualAddress;
    PIMAGE_EXPORT_DIRECTORY img_exp_dir = (PIMAGE_EXPORT_DIRECTORY)exp_dir_base;

    // Obfuscate export table offsets to avoid CAPA detection
    // Instead of: img_exp_dir->AddressOfFunctions (offset 0x1C)
    DWORD func_addr_offset = (0xE << 1);  // 0x1C calculated
    PDWORD func_addr = (PDWORD)(dllbaseaddr + *(DWORD*)(exp_dir_base + func_addr_offset));

    // Instead of: img_exp_dir->AddressOfNames (offset 0x20)
    DWORD func_name_offset = (0x10 << 1);  // 0x20 calculated
    PDWORD func_name_rva = (PDWORD)(dllbaseaddr + *(DWORD*)(exp_dir_base + func_name_offset));

    // Instead of: img_exp_dir->AddressOfNameOrdinals (offset 0x24)
    DWORD ordinal_offset = (0x12 << 1);  // 0x24 calculated
    PWORD func_ordinal = (PWORD)(dllbaseaddr + *(DWORD*)(exp_dir_base + ordinal_offset));

    DWORD export_start = img_data_dir->VirtualAddress;
    DWORD export_end   = export_start + img_data_dir->Size;

    // Obfuscate NumberOfNames access (offset 0x18) to avoid CAPA detection
    DWORD num_names_offset = (0xC << 1);  // 0x18 calculated
    DWORD num_names = *(DWORD*)(exp_dir_base + num_names_offset);

    for (DWORD i = 0; i < num_names; ++i){
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

__attribute__((__annotate__(("substitution,indirectcall"))))
BOOL insert_new_dll(PHASHMAP func_map, PCHAR dll_name) {
    if (!func_map || !dll_name) {
        _err("Invalid parameters to insert_new_dll");
        return FALSE;
    }

    PPEB peb = get_peb_stealth();
    if (!peb) {
        _err("Failed to resolve PEB");
        return FALSE;
    }

    PEB_LDR_DATA* ldr = (PEB_LDR_DATA*)peb->Ldr;

    PBYTE dll_base = ldr_find_module(ldr, dll_name);
    if (!dll_base) {

        fn_LoadLibraryA pLoadLibraryA = (fn_LoadLibraryA)hashmap_get(func_map, (PVOID)xorstr_("LoadLibraryA"), 12);
        dll_base = (PBYTE)pLoadLibraryA(dll_name);

        _inf("Loaded %s at 0x%p", dll_name, dll_base);

        if(dll_base == NULL){ // Failed to load the DLL
            _err("Failed to load module: %s", dll_name);
            return FALSE;
        }
    }

    resolv_functions(func_map, dll_base);

    return TRUE;
}
