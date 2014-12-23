#pragma once

#include <sal.h>
#include <minwindef.h>

class CImage
{
	const void* m_imgBase;
	const void* m_ntHeader;
public:
	CImage(
		__in const void* imgBase
		);

	size_t
	SizeOfImage();

	void*
	ImageBase();

	const void*
	RellocationDir();
	
	const void*
	ExportDir();

	static
	__checkReturn
	bool
	FixRellocations(
		__inout void* newImageBase,
		__inout const void* oldImageBase
		);

	__checkReturn
	void
	Rellocate(
		__in void* newImageBase
		);
	
	__checkReturn
	bool
	CImage::IsForwardedExport(
		__in const void* addr
		);
	
	__checkReturn 
	const void* 
	GetProcAddress(
		__in const char* name
		);

	__checkReturn 
	const void* 
	GetProcAddress(
		__in size_t ordinal
		);

	__checkReturn
	static 
	const void* 
	GetProcAddress(
		__in const char* name,
		__in const void* base,
		__in const void* imgDir,
		__in const void* exportDir
		);

	__checkReturn
	static 
	const void* 
	GetProcAddress(
		__in size_t ordinal,
		__in const void* base,
		__in const void* imgDir,
		__in const void* exportDir
		);
};
