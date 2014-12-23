#pragma once

#include <ntifs.h>
#include "DriverObject.h"
#include "../auto/DerefPtr.hpp"

class CDummyDevice :
	public CDerefObj<DRIVER_OBJECT>
{
public:
	__drv_requiresIRQL(DISPATCH_LEVEL)
	explicit 
	CDummyDevice(
		__in DRIVER_OBJECT* drvObj
		) : CDerefObj(&m_drvObjContainer.DriverObject)
	{
		RtlZeroMemory(m_drvObjContainer.ReservedBefore, sizeof(m_drvObjContainer.ReservedBefore));
		RtlZeroMemory(m_drvObjContainer.ReservedAfter, sizeof(m_drvObjContainer.ReservedAfter));

		m_drvObjContainer.DriverObject = *drvObj;
		m_drvObjContainer.ObjectHeader = *CONTAINING_RECORD(drvObj, _OBJECT_HEADER, Body);
		
		get()->DeviceObject = nullptr;
		get()->FastIoDispatch = nullptr; //LEGACY DRIVER, for now not my target
		for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
			get()->MajorFunction[i] = _IrpPassTrhu;

		(void)ObfReferenceObject(get());
	}

	~CDummyDevice()
	{
		DettachFromDeviceObj();
	}

	static
	__forceinline
	size_t GetSizeOfDeviceExtension()
	{
		return sizeof(void*);
	}

	__checkReturn
	bool AttachToDeviceObj(
		__in DEVICE_OBJECT* trampolineDevObj,
		__in DEVICE_OBJECT* attachToDevObj
		)
	{
		if (!get()->DeviceObject)
		{
			get()->DeviceObject = trampolineDevObj;

			*static_cast<CDummyDevice**>(trampolineDevObj->DeviceExtension) = this;

			m_attachedInfo.AttachedTo = attachToDevObj;
			m_attachedInfo.AttachedTop = IoAttachDeviceToDeviceStack(trampolineDevObj, attachToDevObj);

			//update flags
			if (m_attachedInfo.AttachedTop)
			{
				//copy flags
				trampolineDevObj->AlignmentRequirement = m_attachedInfo.AttachedTop->AlignmentRequirement;
				trampolineDevObj->Characteristics = m_attachedInfo.AttachedTop->Characteristics;
				trampolineDevObj->Flags = m_attachedInfo.AttachedTop->Flags;
				trampolineDevObj->Flags &= ~DO_DEVICE_INITIALIZING;
				//adding itself to stack
				trampolineDevObj->StackSize = m_attachedInfo.AttachedTop->StackSize + 1;
				return true;
			}
		}
		return false;
	}

	void DettachFromDeviceObj()
	{
		if (!get()->DeviceObject)
			return;

		ATTACHED_DEVICES_EXT* dvc_ext = static_cast<ATTACHED_DEVICES_EXT*>(get()->DeviceObject->DeviceExtension);
		if (!dvc_ext)
			return;

		if (!dvc_ext->AttachedTop)
			return;

		IoDetachDevice(dvc_ext->AttachedTop);
		//do not reuse this obj!
		dvc_ext->AttachedTop = nullptr;
		//do not reuse this obj!
		get()->DeviceObject = nullptr;
	}

private:
	static
	__drv_functionClass(DRIVER_DISPATCH)
	__drv_requiresIRQL(PASSIVE_LEVEL)
	__drv_sameIRQL
	NTSTATUS
	_IrpPassTrhu(
		__in struct _DEVICE_OBJECT *DeviceObject,
		__inout struct _IRP *Irp
		)
	{
		DbgBreakPoint();
		DbgPrint("\nattached!!  mj : %p; mn : %p", Irp->Tail.Overlay.CurrentStackLocation->MajorFunction, Irp->Tail.Overlay.CurrentStackLocation->MinorFunction);

		CDummyDevice** dummy = static_cast<CDummyDevice**>(DeviceObject->DeviceExtension);
		return (*dummy)->IrpNext(DeviceObject, Irp);
	}

protected:
	virtual
	__drv_functionClass(DRIVER_DISPATCH)
	__drv_requiresIRQL(PASSIVE_LEVEL)
	__drv_sameIRQL
	NTSTATUS
	IrpNext(
		__in struct _DEVICE_OBJECT *DeviceObject,
		__inout struct _IRP *Irp
		)
	{
		return IrpPassTrhu(DeviceObject, Irp);
	}

	__drv_functionClass(DRIVER_DISPATCH)
	__drv_requiresIRQL(PASSIVE_LEVEL)
	__drv_sameIRQL
	NTSTATUS
	IrpPassTrhu(
		__in struct _DEVICE_OBJECT *DeviceObject,
		__inout struct _IRP *Irp
		)
	{
		IoSkipCurrentIrpStackLocation(Irp);
		return IoCallDriver(m_attachedInfo.AttachedTop, Irp);
	}

private:
	ATTACHED_DEVICES_EXT m_attachedInfo;
	_DRIVEROBJECT m_drvObjContainer;
};

class CHookerDevice :
	public CDummyDevice
{
public:
	__drv_requiresIRQL(DISPATCH_LEVEL)
	CHookerDevice(
		__in DRIVER_OBJECT* drvObj
		) : CDummyDevice(drvObj)
	{
	}
protected:	
	virtual
	__drv_functionClass(DRIVER_DISPATCH)
	__drv_requiresIRQL(PASSIVE_LEVEL)
	__drv_sameIRQL
	NTSTATUS
	IrpNext(
		__in struct _DEVICE_OBJECT *DeviceObject,
		__inout struct _IRP *Irp
		) override
	{
		switch (Irp->Tail.Overlay.CurrentStackLocation->MajorFunction)
		{
		case IRP_MJ_DEVICE_CONTROL:
			return DeviceControl(DeviceObject, Irp);
		default:
			return IrpPassTrhu(DeviceObject, Irp);
		}
	}

	virtual
	__drv_functionClass(DRIVER_DISPATCH)
	__drv_requiresIRQL(PASSIVE_LEVEL)
	__drv_sameIRQL
	NTSTATUS
	DeviceControl(
		__in struct _DEVICE_OBJECT *DeviceObject,
		__inout struct _IRP *Irp
		)
	{
		return IrpPassTrhu(DeviceObject, Irp);
	}
};
