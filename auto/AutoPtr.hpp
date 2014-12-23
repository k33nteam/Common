#pragma once

#include <ntifs.h>
#include <memory>

template<typename TYPE>
class CAutoMem :
	public std::unique_ptr<TYPE>
{
public:
	CAutoMem(
		__in size_t len = 1
		) : std::unique_ptr<TYPE>(static_cast<TYPE*>(malloc(sizeof(TYPE) * len)))
	{
	}
};

template<typename TYPE>
class CScopedObj :
	public std::unique_ptr<TYPE, decltype(&ObfDereferenceObject)>
{
public:
	CScopedObj(
		__in TYPE* obj
		) : std::unique_ptr<TYPE, decltype(&ObfDereferenceObject)>(obj, ObfDereferenceObject)
	{
		ObReferenceObject(obj);
	}
};

template<typename TYPE>
class CScopedHandleObj
{
	TYPE m_obj;
public:
	CScopedHandleObj(
		__in HANDLE hdnl,
		__in ACCESS_MASK mask = GENERIC_READ
		) : m_obj(nullptr)
	{
		ObReferenceObjectByHandle(
			hdnl, 
			mask, 
			nullptr, 
			KernelMode, 
			reinterpret_cast<void**>(&m_obj), 
			nullptr);
	}

	TYPE
	operator->()
	{
		return m_obj;
	}

	TYPE
	Get()
	{
		return m_obj;
	}
};
