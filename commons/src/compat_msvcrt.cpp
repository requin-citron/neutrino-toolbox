#include "neutrino/compat_msvcrt.h"

extern "C" PVOID neutrino_memset(PVOID dest, INT val, SIZE_T len) {
    PBYTE ptr = (PBYTE)dest;
    while (len--) {
        *ptr++ = (BYTE)val;
    }
    return dest;
}

extern "C" PVOID neutrino_memcpy(PVOID dest, PVOID src, SIZE_T len) {
    PBYTE d = (PBYTE)dest;
    PBYTE s = (PBYTE)src;
    while (len--) {
        *d++ = *s++;
    }
    return dest;
}

extern "C" INT neutrino_memcmp(const PVOID s1, const PVOID s2, SIZE_T len) {                                                           
       PBYTE p1 = (PBYTE)s1;                                                                                                              
       PBYTE p2 = (PBYTE)s2;                                                                                                              
       while (len--) {                                                                                                                    
           if (*p1 != *p2) {                                                                                                              
               return *p1 - *p2;                                                                                                          
           }                                                                                                                              
           p1++;                                                                                                                          
           p2++;                                                                                                                          
       }                                                                                                                                  
       return 0;                                                                                                                          
}   

extern "C" PVOID neutrino_memmove(PVOID dest, const PVOID src, SIZE_T len) {                                                           
       PBYTE d = (PBYTE)dest;                                                                                                             
       PBYTE s = (PBYTE)src;                                                                                                              
       if (d < s) {                                                                                                                       
           while (len--) {                                                                                                                
               *d++ = *s++;                                                                                                               
           }                                                                                                                              
       } else {                                                                                                                           
           d += len;                                                                                                                      
           s += len;                                                                                                                      
           while (len--) {                                                                                                                
               *--d = *--s;                                                                                                               
           }                                                                                                                              
       }                                                                                                                                  
       return dest;                                                                                                                       
}  

// Compiler intrinsics - required when compiling without CRT
extern "C" void* memset(void* dest, int val, size_t len) {
    return neutrino_memset(dest, val, len);
}

extern "C" void* memcpy(void* dest, const void* src, size_t len) {
    return neutrino_memcpy(dest, (PVOID)src, len);
}

extern "C" int memcmp(const void* s1, const void* s2, size_t len) {
    return neutrino_memcmp((PVOID)s1, (PVOID)s2, len);
}

extern "C" void* memmove(void* dest, const void* src, size_t len) {
    return neutrino_memmove(dest, (PVOID)src, len);
}