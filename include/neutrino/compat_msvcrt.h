#ifndef NEUTRINO_COMPAT_MSVCRT_H
#define NEUTRINO_COMPAT_MSVCRT_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// Memory functions
PVOID neutrino_memset(PVOID dest, INT val, SIZE_T len);
PVOID neutrino_memcpy(PVOID dest, const PVOID src, SIZE_T len);
PVOID neutrino_memmove(PVOID dest, const PVOID src, SIZE_T len);
INT neutrino_memcmp(const PVOID s1, const PVOID s2, SIZE_T len);

#ifdef __cplusplus
}
#endif

#endif //NEUTRINO_COMPAT_MSVCRT_H
