#ifndef PEB_WALKING_H
#define PEB_WALKING_H

#include <windows.h>
#include "debug.h"
#include "hashmap.h"
#include "utils.h"
#include "pebteb.h"

#define PEB_WALK_HASH_MAP_SIZE 4096*4*4

PBYTE ldr_find_module(PPEB_LDR_DATA ldr, PCHAR target_module_name);
PHASHMAP init_function_map();
VOID resolv_functions(PHASHMAP func_map, PBYTE dllbaseaddr);
BOOL insert_new_dll(PHASHMAP func_map, PCHAR dll_name);

#endif //PEB_WALKING_H