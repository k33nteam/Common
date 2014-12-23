#pragma once

#include "../usr_common.h"

#include <memory>

#include <Psapi.h>
#include <string>
#include <tlhelp32.h>
#include <shellapi.h>
#include <Windows.h>

class CProcessInsider
{
public:
	static
	__checkReturn
	DWORD
	GetProcessId(
		__in_z const WCHAR* procName
		)
	{
		std::unique_ptr<void, decltype(&CloseHandle)> h_snap(
			CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0),
			CloseHandle);

		if (!h_snap.get())
			return 0;

		PROCESSENTRY32 pe32 = { 0 };
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(h_snap.get(), &pe32))
			return 0;

		do
		{
			WCHAR* svc_name = const_cast<WCHAR*>(pe32.szExeFile + (1 + std::wstring(pe32.szExeFile).find_last_of(L"\\", wcslen(pe32.szExeFile))));
			if (!_wcsicmp(procName, svc_name))
				return pe32.th32ProcessID;
		} while (Process32Next(h_snap.get(), &pe32));

		return 0;
	}

	static
	__checkReturn
	const void*
	GetModuleBase(
		__in DWORD procId,
		__in_z WCHAR* modName
		)
	{
		std::unique_ptr<void, decltype(&CloseHandle)> h_snap(
			CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procId),
			CloseHandle);

		if (!h_snap.get())
			return false;

		MODULEENTRY32 m32 = { 0 };
		m32.dwSize = sizeof(MODULEENTRY32);
		if (!Module32First(h_snap.get(), &m32))
			return false;

		do
		{
			if (!_wcsicmp(m32.szModule, modName))
				return m32.modBaseAddr;
		} while (Module32Next(h_snap.get(), &m32));

		return nullptr;
	}
};
