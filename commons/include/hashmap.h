#ifndef HASHMAP_H
#define HASHMAP_H

#include <windows.h>

#define HASHMAP_DEFAULT_SIZE 64

typedef struct _HASHMAP_ENTRY {
    PVOID key;
    SIZE_T key_len;
    PVOID value;
    struct _HASHMAP_ENTRY* next;
} HASHMAP_ENTRY, *PHASHMAP_ENTRY;

typedef struct _HASHMAP {
    PHASHMAP_ENTRY* buckets;
    DWORD size;
    DWORD count;
} HASHMAP, *PHASHMAP;

DWORD hash_fnv1a(PBYTE data, SIZE_T len);

PHASHMAP hashmap_create(DWORD size);
VOID hashmap_destroy(PHASHMAP map);
BOOL hashmap_insert(PHASHMAP map, PVOID key, SIZE_T key_len, PVOID value);
PVOID hashmap_get(PHASHMAP map, PVOID key, SIZE_T key_len);
BOOL hashmap_remove(PHASHMAP map, PVOID key, SIZE_T key_len);

#endif //HASHMAP_H
