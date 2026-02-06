#include "neutrino/utils.h"

PWCHAR neutrino_char_to_wchar(PCHAR str) {
    if (str == NULL) return NULL;

    INT len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (len == 0) return NULL;

    PWCHAR wstr = (PWCHAR)mcalloc(len * sizeof(WCHAR));
    if (wstr == NULL) return NULL;

    MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len);
    return wstr;
}

__attribute__((__annotate__(("substitution"))))
PCHAR neutrino_wchar_to_char(PWCHAR wstr) {
    if (wstr == NULL) return NULL;

    INT len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (len == 0) return NULL;

    PCHAR str = (PCHAR)mcalloc(len);
    if (str == NULL) return NULL;

    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    return str;
}

__attribute__((__annotate__(("substitution"))))
UINT hash_x65599(PCHAR string, UINT len)
{
    UINT hash = 0;
    for(UINT i = 0; i < len; ++i)
    {
       hash = 65599 * hash + string[i];
    }
    return hash ^ (hash >> 16);
}
