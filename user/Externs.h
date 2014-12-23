#pragma once

#include "../usr_common.h"
#include "../undoc/SystemInfo.h"
#include <TlHelp32.h>

#ifndef WIN64
#include <rtlsupportapi.h>
#endif

//http://recon.cx/2013/slides/Recon2013-Alex%20Ionescu-I%20got%2099%20problems%20but%20a%20kernel%20pointer%20ain't%20one.pdf
//I Got 99 Problem But a Kernel Pointer Ain¡¯t One : There¡¯s an info leak party at Ring 0

extern "C"
NTSTATUS
WINAPI 
NtWriteVirtualMemory(
	__in HANDLE hProcess,
	__in void* lpBaseAddress,
	__in_bcount(nSize) LPCVOID lpBuffer,
	__in size_t nSize,
	__out_opt size_t * lpNumberOfBytesWritten
	);

extern "C"
NTSTATUS
WINAPI
NtReadVirtualMemory(
	__in HANDLE hProcess,
	__in_bcount(nSize) const void* lpBaseAddress,
	__inout_bcount(nSize) void* lpBuffer,
	__in size_t nSize,
	__out_opt size_t * lpNumberOfBytesReaded
	);

extern "C"
NTSTATUS
WINAPI
RtlAdjustPrivilege(
	__in unsigned long Privilege,
	__in BOOLEAN Enable,
	__in BOOLEAN CurrentThread,
	__out BOOLEAN* Enabled
	);

extern "C"
NTSTATUS
WINAPI
NtQueryIntervalProfile(
	__in enum KPROFILE_SOURCE payloadType,
	__inout void* Payload
	);

typedef NTSTATUS(NTAPI* LdrHotPatchRoutine_t)(
	struct LDR_HOT_PATCH_LOADDLL*
		);

typedef void(NTAPI* ExitProcess_t)(
	int
	);

extern "C"
NTSTATUS
WINAPI
RtlUnicodeToMultiByteN(
    _Out_writes_bytes_to_(MaxBytesInMultiByteString, *BytesInMultiByteString) PCHAR MultiByteString,
    _In_ ULONG MaxBytesInMultiByteString,
    _Out_opt_ PULONG BytesInMultiByteString,
    _In_reads_bytes_(BytesInUnicodeString) PCWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
	);

extern "C"
SOCKET
PASCAL FAR
WSASocketW(
	_In_ int af,
	_In_ int type,
	_In_ int protocol,
	_In_opt_ void* lpProtocolInfo,
	_In_ size_t g,
	_In_ size_t dwFlags
	);
