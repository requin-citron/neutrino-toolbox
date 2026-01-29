#include "config.h"

extern "C" VOID __main(){
    return;
}

VOID parse_linklist_module(PLIST_ENTRY linked_lst){
    PLIST_ENTRY curr  = linked_lst->Flink;
    

    while(linked_lst != curr){
        PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(curr, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
        
        _inf("Module Base: 0x%p", entry->DllBase);
        _inf("Module Size: 0x%lx", entry->SizeOfImage);
        
        WCHAR buffer[MAX_PATH] = {0};
        wcsncpy_s(buffer, entry->BaseDllName.Buffer, entry->BaseDllName.Length / sizeof(WCHAR));
        _inf("Module Name: %ws", buffer);
        _inf("---------------------------");

        curr = curr->Flink;
    }

    return;
}

INT main(){

    PEB* peb          = (PEB*)__readgsqword(0x60);
    PEB_LDR_DATA* ldr = (PEB_LDR_DATA*)peb->Ldr;

    _inf("hash_x65599(\"kernel32.dll\") = 0x%X", hash_x65599((PCHAR)"kernel32.dll", sizeof("kernel32.dll") - 1));

    _inf("PEB address: 0x%p", peb);
    _inf("Ldr address: 0x%p", ldr);
    _inf("---------------------------");

    parse_linklist_module(&ldr->InLoadOrderModuleList);

    return 0;
}