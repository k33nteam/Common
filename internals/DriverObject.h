#pragma once

#include <ntifs.h>

struct ATTACHED_DEVICES_EXT
{
	DEVICE_OBJECT* AttachedTo;
	DEVICE_OBJECT* AttachedTop;
};

#pragma pack(push, 1)
/*
kd> dt _OWN_OBJECT_HEADER
+0x000 PointerCount     : Int8B
+0x008 HandleCount      : Int8B
+0x008 NextToFree       : Ptr64 Void
+0x010 Type             : Ptr64 _OBJECT_TYPE
+0x018 NameInfoOffset   : UChar
+0x019 HandleInfoOffset : UChar
+0x01a QuotaInfoOffset  : UChar
+0x01b Flags            : UChar
+0x01c ObjectCreateInfo : Ptr64 Void
+0x01c QuotaBlockCharged : Ptr64 Void
+0x024 SecurityDescriptor : Ptr64 Void
+0x02c Body             : _QUAD
*/
struct _OBJECT_HEADER
{
	LONG_PTR PointerCount;
	union
	{
		LONG_PTR HandleCount;
		PVOID NextToFree;
	};
	void* Type;
	UCHAR NameInfoOffset;
	UCHAR HandleInfoOffset;
	UCHAR QuotaInfoOffset;
	UCHAR Flags;
	union
	{
		PVOID ObjectCreateInfo;
		PVOID QuotaBlockCharged;
	};
	PVOID SecurityDescriptor;
	QUAD Body;
};

struct _DRIVEROBJECT
{
	//paranoic but needed, due hacky way for what it is used ..
	void* ReservedBefore[0x10];

	_OBJECT_HEADER ObjectHeader;
	DRIVER_OBJECT DriverObject;

	//paranoic but needed, due hacky way for what it is used ..
	void* ReservedAfter[0x10];
};
#pragma pack(pop)
