#pragma once

#include <ntifs.h>
#include "../auto/AutoPtr.hpp"
#include "../auto/DerefPtr.hpp"
#include "../internals/DummyDriveObject.hpp"

template<class DEVICE_HOOKER>
class CAttach2Device :
	CScopedObj<DRIVER_OBJECT>
{
public:
	//temporary
	//!object \Device
	static
	DRIVER_OBJECT*
	GetTargetDriverObjetct(
		__in_z const WCHAR* drvName
		)
	{
		UNICODE_STRING drv_name;
		RtlInitUnicodeString(&drv_name, drvName);
		DbgPrint("\nfind device : %ws\n", drvName);
		
		FILE_OBJECT* file_obj;
		DEVICE_OBJECT* dvc_obj;
		NTSTATUS status = IoGetDeviceObjectPointer(&drv_name, 0, &file_obj, &dvc_obj);
		if (!NT_SUCCESS(status))
			return nullptr;
		
		DbgPrint("\nGOTIT : FileObject %p, DeviceObject %p\n", file_obj, dvc_obj);
		CDerefObj<FILE_OBJECT> deref_fobj(file_obj);
		
		//DEVICE_OBJECT* dvc_obj = (DEVICE_OBJECT*)0xfffffa8001fc7060;
		return dvc_obj->DriverObject;
	}

	__drv_requiresIRQL(DISPATCH_LEVEL)
	CAttach2Device(
		__in DRIVER_OBJECT* drvObj,
		__in_z WCHAR* drvName
		) : m_hookerDevice(drvObj),
			CAutoObRef(drvObj)
	{
		RtlInitUnicodeString(&m_drvName, drvName);
	}

	__drv_requiresIRQL(PASSIVE_LEVEL)
	~CAttach2Device()
	{
		Detach();
	}

	__drv_requiresIRQL(PASSIVE_LEVEL)
	__checkReturn
	bool Attach()
	{
		if (!m_trampolineDevice)
		{
			FILE_OBJECT* file_obj = nullptr;
			DEVICE_OBJECT* dvc_obj;
			NTSTATUS status = IoGetDeviceObjectPointer(&m_drvName, 0, &file_obj, &dvc_obj);
			//NTSTATUS status=STATUS_SUCCESS;  dvc_obj = (DEVICE_OBJECT*)0xfffffa8001fc7060;
			if (NT_SUCCESS(status))
			{
				DbgPrint("\nGOTIT : FileObject %p, DeviceObject %p", file_obj, dvc_obj);
				CDerefObj<FILE_OBJECT> deref_fobj(file_obj);

				status = IoCreateDevice(GetRef(), CDummyDevice::GetSizeOfDeviceExtension(), NULL, FILE_DEVICE_UNKNOWN, 0, FALSE, &m_trampolineDevice);
				if (NT_SUCCESS(status))
				{
					//be more independent in current driver, redirect it to dummy driver
					m_trampolineDevice->DriverObject = m_hookerDevice.GetRef();
					//attach dummy driver, with custom major hooks!
					return m_hookerDevice.AttachToDeviceObj(m_trampolineDevice, dvc_obj);
				}
			}
		}
		return false;
	}

	__drv_requiresIRQL(PASSIVE_LEVEL)
	bool Detach()
	{
		if (m_trampolineDevice)
		{
			m_hookerDevice.DettachFromDeviceObj();
			//restore driver object
			m_trampolineDevice->DriverObject = GetRef();
			IoDeleteDevice(m_trampolineDevice);
			//do not reuse this obj!
			m_trampolineDevice = nullptr;
			return true;
		}
		return false;
	}

protected:
	UNICODE_STRING m_drvName;
	DEVICE_HOOKER m_hookerDevice;

private:
	DEVICE_OBJECT* m_trampolineDevice;
};
