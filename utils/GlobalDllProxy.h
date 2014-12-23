#pragma once

#include <sal.h>
#include <minwindef.h>

typedef HMODULE(WINAPI*GetModuleBase)(const char*);

namespace GlobalDllProxy
{
	const void*
	GetProcAddr(
		__in const void* base,
		__in const char* procName
		);

	__checkReturn
	const void*
	ResolveProcAddress(
		__in const GetModuleBase getModuleBaseA,
		__in const char* moduleName,
		__in const char* procName
		);
};
