#pragma once

#include <sal.h>
#include <minwindef.h>

#pragma pack(push, 1)

struct RELATIVE_UNICODE_STRING
{
	unsigned short DeltaToName;
	unsigned short Length;
};
struct LDR_HOT_PATCH_LOADDLL
{
#define NTDLL_STRING L"ntdll.dll"
	LDR_HOT_PATCH_LOADDLL(
		__in_z const WCHAR* pathToDll
		) : Mask(-1),
			LoadedDll({ offsetof(LDR_HOT_PATCH_LOADDLL, Ntdll), _countof(Ntdll) - sizeof(Ntdll[0]) })
	{
		Dummy = LoadedDll;

		size_t len = wcslen(pathToDll);
		DllToLoad = { offsetof(LDR_HOT_PATCH_LOADDLL, CustomDllPath), len * sizeof(pathToDll[0]) };

		memcpy(Ntdll, NTDLL_STRING, sizeof(NTDLL_STRING));
		memcpy(CustomDllPath, pathToDll, DllToLoad.Length + sizeof(pathToDll[0]));
	}
	LDR_HOT_PATCH_LOADDLL(
		__in const LDR_HOT_PATCH_LOADDLL& pattern
		)
	{
		*this = pattern;
		memcpy(CustomDllPath, pattern.CustomDllPath, sizeof(CustomDllPath));
	}
protected:
	ULONG64 Mask;
	RELATIVE_UNICODE_STRING DllToLoad;
	RELATIVE_UNICODE_STRING Dummy;
	RELATIVE_UNICODE_STRING LoadedDll;
	WCHAR Ntdll[sizeof(NTDLL_STRING)];
	WCHAR CustomDllPath[0x100];
};

#pragma pack(pop)
