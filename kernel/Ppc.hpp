#pragma once

#include "../drv_common.h"
#include <memory>


class CPpc
{
	static
	void
	IoFreeWorkItemEx(
		__in void* IoWorkItem
		)
	{
		IoFreeWorkItem(static_cast<PIO_WORKITEM>(IoWorkItem));
	}
public:
	_IRQL_requires_max_(DISPATCH_LEVEL)
	explicit CPpc(
		__in DEVICE_OBJECT* deviceObj
		) : m_pWorkItem(IoAllocateWorkItem(deviceObj), IoFreeWorkItemEx)
	{
	}

	_IRQL_requires_max_(DISPATCH_LEVEL)
	__checkReturn
	bool
	Run()
	{
		if (!m_pWorkItem.get())
			return false;
		IoQueueWorkItem(static_cast<PIO_WORKITEM>(m_pWorkItem.get()), StaticPpc, DelayedWorkQueue, this);
		return true;
	}

protected:
	_IRQL_requires_max_(PASSIVE_LEVEL)
	virtual
	void
	CustomPpc(
		__in DEVICE_OBJECT* devObj
		) = 0;

private:
	_IRQL_requires_max_(PASSIVE_LEVEL)
	static
	void
	StaticPpc(
		__in DEVICE_OBJECT* devObj,
		__in void* context
		)
	{
		if (!context)
			return;
		CPpc* ppc = static_cast<CPpc*>(context);
		ppc->CustomPpc(devObj);
	}

protected:
	std::unique_ptr<void, decltype(&IoFreeWorkItemEx)> m_pWorkItem;
};
