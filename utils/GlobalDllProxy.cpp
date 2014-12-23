#include "GlobalDllProxy.h"

#include "../internals/Image.h"

namespace GlobalDllProxy
{
	const void*
	GetProcAddr(
		__in const void* base,
		__in const char* procName
		)
	{
		CImage img(base);
		return img.GetProcAddress(procName);
	}

	__checkReturn
	const void*
	ResolveProcAddress(
		__in const GetModuleBase getModuleBaseA,
		__in const char* moduleName,
		__in const char* procName)
	{
		const void* module = getModuleBaseA(moduleName);
		if (!module)
			return nullptr;

		CImage img(module);
		const void* addr = img.GetProcAddress(procName);
		if (!addr)
			return nullptr;

		if (!img.IsForwardedExport(addr))
			return addr;

		char dll[0x10] = { 0 };
		const char* proc_name = static_cast<const char*>(addr) + 1;
		for (size_t i = 0; i < sizeof(dll); i++)
			if ('.' == proc_name++[0])
				break;

		if (proc_name - addr > sizeof(dll))
			return nullptr;
		
		memcpy(dll, addr, &proc_name[-1] - addr);

		return ResolveProcAddress(
			getModuleBaseA,
			dll,
			proc_name
			);
	}
};
