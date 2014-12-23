#pragma once

#include <sal.h>
#include <memory>

#include "../undoc/SystemInfo.h"

extern "C"
NTSTATUS
NTAPI 
ZwQuerySystemInformation(
    __in SYSTEM_INFORMATION_CLASS_EX systemInformationClass,
    __out void* systemInformation,
    __in size_t systemInformationLength,
    __inout_opt size_t* returnLength
    );

template<typename SYS_TYPE>
class CSystemInfo :
	public std::unique_ptr<SYS_TYPE>
{
public:
	CSystemInfo(
		__in SYSTEM_INFORMATION_CLASS_EX sysClass
		) :
		std::unique_ptr<SYS_TYPE>(Query(sysClass))
	{
	}

	static
	__checkReturn
	bool
	QuerInfo(
		__in SYSTEM_INFORMATION_CLASS_EX sysClass,
		__inout_bcount(size) SYS_TYPE* info,
		__in size_t size
		)
	{
		if (size < sizeof(*info))
			return false;

		return (STATUS_INFO_LENGTH_MISMATCH == ZwQuerySystemInformation(
			sysClass,
			info,
			size,
			nullptr));
	}
protected:
	__checkReturn
	std::nullptr_t
	Query(
		__in SYSTEM_INFORMATION_CLASS_EX sysClass
		)
	{
		size_t size = sizeof(SYS_TYPE) + 0x1000;
		void* info = malloc(size);
		if (!info)
			return nullptr;

		for (NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH; 
			STATUS_INFO_LENGTH_MISMATCH == status; 
			size *= 2)
		{
			status = ZwQuerySystemInformation(
				sysClass,
				info,
				size,
				nullptr);

			info = realloc(info, size * 2);
			if (!info)
				break;
		}

		return static_cast<std::nullptr_t>(info);
	}
};
