#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

#define mcalloc(size) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size)
#define mcfree(ptr) HeapFree(GetProcessHeap(), 0, ptr)

PWCHAR char_to_wchar(PCHAR str);
PCHAR wchar_to_char(PWCHAR wstr);
UINT hash_x65599(PCHAR string, UINT len);

#endif //UTILS_H