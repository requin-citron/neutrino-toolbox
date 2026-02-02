#ifndef NEUTRINO_UTILS_H
#define NEUTRINO_UTILS_H

#include <windows.h>

#define mcalloc(size) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size)
#define mcfree(ptr) HeapFree(GetProcessHeap(), 0, ptr)

PWCHAR neutrino_char_to_wchar(PCHAR str);
PCHAR neutrino_wchar_to_char(PWCHAR wstr);
UINT hash_x65599(PCHAR string, UINT len);

#endif //NEUTRINO_UTILS_H
