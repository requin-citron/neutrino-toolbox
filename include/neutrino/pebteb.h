#ifndef NEUTRINO_PEBTEB_H
#define NEUTRINO_PEBTEB_H

#include <windows.h>

// https://malwaretech.com/wiki/locating-modules-via-the-peb-x64

typedef struct _PEB_LDR_DATA
{
    ULONG Length;                                // +0x00
    UCHAR Initialized;                           // +0x04
    PVOID SsHandle;                              // +0x08
    LIST_ENTRY InLoadOrderModuleList;            // +0x10
    LIST_ENTRY InMemoryOrderModuleList;          // +0x20
    LIST_ENTRY InInitializationOrderModuleList;  // +0x30
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB
{
    UCHAR InheritedAddressSpace;            // +0x00
    UCHAR ReadImageFileExecOptions;         // +0x01
    UCHAR BeingDebugged;                    // +0x02
    UCHAR BitField;                         // +0x03
    ULONG ImageUsesLargePages : 1;          // +0x03
    ULONG IsProtectedProcess : 1;           // +0x03
    ULONG IsLegacyProcess : 1;              // +0x03
    ULONG IsImageDynamicallyRelocated : 1;  // +0x03
    ULONG SpareBits : 4;                    // +0x03
    PVOID Mutant;                           // +0x08
    PVOID ImageBaseAddress;                 // +0x10
    PPEB_LDR_DATA Ldr;                     // +0x18 (This is what we're interested in)

    // Snipped (we don't need the rest of the PEB defined)

} PEB, *PPEB;

typedef struct _UNICODE_STRING {
    USHORT Length;                             // +0x00
    USHORT MaximumLength;                      // +0x02
    PWSTR  Buffer;                             // +0x08
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;               // +0x00
    LIST_ENTRY InMemoryOrderLinks;             // +0x10
    LIST_ENTRY InInitializationOrderLinks;     // +0x20
    PVOID DllBase;                             // +0x30
    PVOID EntryPoint;                          // +0x38
    ULONG SizeOfImage;                         // +0x40
    UNICODE_STRING FullDllName;                // +0x48
    UNICODE_STRING BaseDllName;                // +0x58
    ULONG Flags;                               // +0x68
    USHORT LoadCount;                          // +0x6C
    USHORT TlsIndex;                           // +0x6E
    LIST_ENTRY HashLinks;                      // +0x70
    ULONG TimeDateStamp;                       // +0x80
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;         // +0x00
    HANDLE UniqueThread;          // +0x08
} CLIENT_ID, *PCLIENT_ID;

typedef struct _TEB
{
    NT_TIB NtTib;                 // +0x00
    PVOID EnvironmentPointer;     // +0x38
    CLIENT_ID ClientId;           // +0x40
    PVOID ActiveRpcHandle;        // +0x50
    PVOID ThreadLocalStoragePointer; // +0x58
    PPEB ProcessEnvironmentBlock; // +0x60 (This is what we need!)
    // Snipped (rest not needed)
} TEB, *PTEB;

#endif //NEUTRINO_PEBTEB_H
