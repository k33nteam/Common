#pragma once

#include <ntifs.h>

typedef void(__fastcall *MINIPACKETCALLBACK)(
	__in struct IO_MINI_COMPLETION_PACKET_USER* miniPacket,
	__inout void* context
	);

EXTERN_C 
NTKERNELAPI 
void* 
NTAPI PsGetThreadTeb(
	__in const PETHREAD thread
	);

EXTERN_C 
NTKERNELAPI 
void* 
NTAPI PsGetProcessWow64Process(
	__in const PEPROCESS process
	);

EXTERN_C
NTKERNELAPI
void* 
NTAPI PsGetThreadTeb(
	__in PETHREAD thread
	);

EXTERN_C
NTKERNELAPI 
void* 
NTAPI IoFreeMiniCompletionPacket(
	__in const void* miniPacket
	);

EXTERN_C
NTKERNELAPI 
void* 
NTAPI IoAllocateMiniCompletionPacket(
	__in MINIPACKETCALLBACK miniPacketCallback,
	__in const void* context
	);

EXTERN_C
NTKERNELAPI 
void 
NTAPI IoSetIoCompletionEx(
	__inout void* completitionPort,
	__in const void* keyContext,
	__in const void* apcContext,
	__in ULONG_PTR ioStatus,
	__in ULONG_PTR ioStatusInformation,
	__in bool allocPacketInfo,
	__in const void* ioMiniCoompletitionPacketUser
	);

EXTERN_C
NTSTATUS
NTAPI
RtlCreateUserThread(
	__in HANDLE processHandle,
	__in SECURITY_DESCRIPTOR* securityDescriptor,
	__in BOOLEAN createSuspended,
	__in ULONG stackZeroBits,
	__inout_opt size_t* stackReserved,
	__inout_opt size_t* stackCommit,
	__in const void* startAddress,
	__in void* startParameter,
	__inout HANDLE* threadHandle,
	__inout_opt CLIENT_ID* clientID
	);
typedef void(NTAPI * PKNORMAL_ROUTINE)(
	__in_opt void* normalContext,
	__in_opt void* systemArgument1,
	__in_opt void* systemArgument2
	);

typedef void(NTAPI * PKKERNEL_ROUTINE)(
	__in KAPC* apc,
	__inout_opt PKNORMAL_ROUTINE* normalRoutine,
	__inout_opt void** normalContext,
	__inout_opt void** systemArgument1,
	__inout_opt void** systemArgument2
	);

typedef void(NTAPI * PKRUNDOWN_ROUTINE)(
	__in KAPC* apc
	);

EXTERN_C
void
NTAPI KeInitializeApc(
	__in KAPC* apc,
	__in PETHREAD thread,
	__in enum KAPC_ENVIRONMENT environment,
	__in PKKERNEL_ROUTINE kernelRoutine,
	__in_opt PKRUNDOWN_ROUTINE rundownRoutine,
	__in_opt PKNORMAL_ROUTINE rormalRoutine,
	__in KPROCESSOR_MODE processorMode,
	__in void* normalContext
	);

EXTERN_C
BOOLEAN
NTAPI
KeInsertQueueApc(
	__in KAPC* apc,
	__in_opt void* systemArgument1,
	__in_opt void* systemArgument2,
	__in KPRIORITY increment
	);
/*
typedef BOOLEAN(NTAPI *KeRemoveQueueApc)(
__in PKAPC Apc
);
*/

EXTERN_C
BOOLEAN
NTAPI
KeRemoveQueueApc(
	__in KAPC* apc
	);

extern "C"
int 
NTAPI 
vsprintf_t(
	__out_bcount(size) char * buff, 
	__in size_t size,
	__in const char * fmt, 
	__in va_list _ArgList
	);

extern "C"
NTSTATUS
NTAPI 
ZwQuerySystemInformation(
    __in enum SYSTEM_INFORMATION_CLASS_EX systemInformationClass,
    __out void* systemInformation,
    __in size_t systemInformationLength,
    __inout_opt size_t* returnLength
    );
