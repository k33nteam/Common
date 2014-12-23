#pragma once

#include <ntifs.h>
#include <memory>

class CDerefHandle :
	public std::unique_ptr<void, decltype(&NtClose)>
{
public:
	CDerefHandle(
		__in HANDLE handle
		) : std::unique_ptr<void, decltype(&NtClose)>(handle, NtClose)
	{
	}
};

template<typename TYPE>
class CDerefObj :
	public std::unique_ptr<TYPE, decltype(&ObfDereferenceObject)>
{
public:
	CDerefObj(
		__in void* obj
		) : std::unique_ptr<TYPE, decltype(&ObfDereferenceObject)>(static_cast<TYPE*>(obj), ObfDereferenceObject)
	{
	}
};
