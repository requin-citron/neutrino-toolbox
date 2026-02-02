#include "hashmap.h"
#include "utils.h"

DWORD hash_fnv1a(PBYTE data, SIZE_T len) {
    DWORD hash = 0x811c9dc5;
    for (SIZE_T i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 0x01000193;
    }
    return hash;
}

PHASHMAP hashmap_create(DWORD size) {
    if (size == 0) size = HASHMAP_DEFAULT_SIZE;

    PHASHMAP map = (PHASHMAP)mcalloc(sizeof(HASHMAP));
    if (!map) return NULL;

    map->buckets = (PHASHMAP_ENTRY*)mcalloc(size * sizeof(PHASHMAP_ENTRY));
    if (!map->buckets) {
        mcfree(map);
        return NULL;
    }

    map->size  = size;
    map->count = 0;
    return map;
}

VOID hashmap_destroy(PHASHMAP map) {
    if (!map) return;

    for (DWORD i = 0; i < map->size; i++) {
        PHASHMAP_ENTRY entry = map->buckets[i];
        while (entry) {
            PHASHMAP_ENTRY next = entry->next;
            mcfree(entry);
            entry = next;
        }
    }

    mcfree(map->buckets);
    mcfree(map);
}

BOOL hashmap_insert(PHASHMAP map, PVOID key, SIZE_T key_len, PVOID value) {
    if (!map || !key) return FALSE;

    DWORD index = hash_fnv1a((PBYTE)key, key_len) % map->size;

    PHASHMAP_ENTRY entry = map->buckets[index];
    while (entry) {
        if (entry->key_len == key_len &&
            RtlCompareMemory(entry->key, key, key_len) == key_len) {
            entry->value = value;
            return TRUE;
        }
        entry = entry->next;
    }

    PHASHMAP_ENTRY new_entry = (PHASHMAP_ENTRY)mcalloc(sizeof(HASHMAP_ENTRY));
    if (!new_entry) return FALSE;

    new_entry->key      = key;
    new_entry->key_len  = key_len;
    new_entry->value    = value;
    new_entry->next     = map->buckets[index];
    map->buckets[index] = new_entry;
    map->count++;

    return TRUE;
}

PVOID hashmap_get(PHASHMAP map, PVOID key, SIZE_T key_len) {
    if (!map || !key) return NULL;

    DWORD index = hash_fnv1a((PBYTE)key, key_len) % map->size;

    PHASHMAP_ENTRY entry = map->buckets[index];
    while (entry) {
        if (entry->key_len == key_len &&
            RtlCompareMemory(entry->key, key, key_len) == key_len) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

BOOL hashmap_remove(PHASHMAP map, PVOID key, SIZE_T key_len) {
    if (!map || !key) return FALSE;

    DWORD index = hash_fnv1a((PBYTE)key, key_len) % map->size;

    PHASHMAP_ENTRY* prev = &map->buckets[index];
    PHASHMAP_ENTRY entry = *prev;

    while (entry) {
        if (entry->key_len == key_len &&
            RtlCompareMemory(entry->key, key, key_len) == key_len) {
            *prev = entry->next;
            mcfree(entry);
            map->count--;
            return TRUE;
        }
        prev = &entry->next;
        entry = entry->next;
    }

    return FALSE;
}

#if _DEBUG == 1

VOID hashmap_print(PHASHMAP map) {
    if (!map) return;

    for (DWORD i = 0; i < map->size; i++) {
        PHASHMAP_ENTRY entry = map->buckets[i];
        if (entry) {
            _inf("Bucket %lu:", i);
            while (entry) {
                _inf("  Key: %s, Key Length: %llu, Value: %p", entry->key, entry->key_len, entry->value);
                entry = entry->next;
            }
        }
    }
}

#endif