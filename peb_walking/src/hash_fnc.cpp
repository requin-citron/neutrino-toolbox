#include "hash_fnc.h"

// A hash function with multiplier 65599 (from Red Dragon book)
UINT hash_x65599(PCHAR string, UINT len)
{
    UINT hash = 0;
    for(UINT i = 0; i < len; ++i)
    {
       hash = 65599 * hash + string[i];
    }
    return hash ^ (hash >> 16);
}