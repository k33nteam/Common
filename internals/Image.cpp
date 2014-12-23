#include "Image.h"

#define MEMBER(cast, ptr, member) reinterpret_cast<cast*>(reinterpret_cast<size_t>(ptr) + static_cast<size_t>(member))

#define NT_HEADER static_cast<const IMAGE_NT_HEADERS*>(m_ntHeader)

CImage::CImage(
	__in const void* imgBase
	) : m_imgBase(imgBase),
		m_ntHeader(nullptr)
{
	const IMAGE_DOS_HEADER* dos_header = reinterpret_cast<const IMAGE_DOS_HEADER*>(imgBase);
	if (IMAGE_DOS_SIGNATURE != dos_header->e_magic)
		return;

	const IMAGE_NT_HEADERS* nt_header = MEMBER(IMAGE_NT_HEADERS, dos_header, dos_header->e_lfanew);
	if (IMAGE_NT_SIGNATURE != nt_header->Signature)
		return;

	if (IMAGE_NT_OPTIONAL_HDR_MAGIC != nt_header->OptionalHeader.Magic)
		return;

	m_ntHeader = nt_header;
}

size_t
CImage::SizeOfImage()
{
	if (!m_ntHeader)
		return 0;
	return NT_HEADER->OptionalHeader.SizeOfImage;
}

void*
CImage::ImageBase()
{
	if (!m_ntHeader)
		return nullptr;
	return reinterpret_cast<void*>(NT_HEADER->OptionalHeader.ImageBase);
}

const void*
CImage::RellocationDir()
{
	return &NT_HEADER->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
}
	
const void*
CImage::ExportDir()
{
	return &NT_HEADER->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
}

__checkReturn
bool 
CImage::FixRellocations(
	__inout void* newImageBase,
	__inout const void* oldImageBase
	)
{
	size_t module_base = reinterpret_cast<size_t>(oldImageBase);
	size_t new_module_base = reinterpret_cast<size_t>(newImageBase);
	const IMAGE_DATA_DIRECTORY* dataDir = static_cast<const IMAGE_DATA_DIRECTORY*>(CImage(newImageBase).RellocationDir());
	for (size_t i = 0; i < dataDir->Size; )
	{
		IMAGE_BASE_RELOCATION* block = reinterpret_cast<IMAGE_BASE_RELOCATION*>(new_module_base + dataDir->VirtualAddress + i);
		if (!block->SizeOfBlock)
			return false;

		unsigned short last_off = 0;
		unsigned short* relloc_t = reinterpret_cast<unsigned short*>(block + 1);
		for (size_t j = 0; j <= (block->SizeOfBlock - sizeof(*block)) / sizeof(relloc_t[0]); j++)
		{
			unsigned short off = (relloc_t[j] & 0xFFF);
			if (last_off >= off)
				continue;
			last_off = off;

			size_t* relloc = reinterpret_cast<size_t*>(new_module_base + block->VirtualAddress + off);
			*relloc = new_module_base + (*relloc - module_base);
		}

		i += block->SizeOfBlock;
	}
	return true;
}

__checkReturn
void
CImage::Rellocate(
	__in void* newImageBase
	)
{
	memcpy(newImageBase, m_imgBase, NT_HEADER->OptionalHeader.SizeOfImage);
	FixRellocations(newImageBase, m_imgBase);
}

__checkReturn 
const void* 
CImage::GetProcAddress(
	__in const char* name
	)
{
	return GetProcAddress(
		name,
		m_imgBase, 
		reinterpret_cast<const IMAGE_DATA_DIRECTORY*>(&NT_HEADER->OptionalHeader.DataDirectory),
		MEMBER(IMAGE_EXPORT_DIRECTORY, m_imgBase, static_cast<const IMAGE_DATA_DIRECTORY*>(ExportDir())->VirtualAddress));
}

__checkReturn 
const void* 
CImage::GetProcAddress(
	__in size_t ordinal
	)
{
	return GetProcAddress(
		ordinal,
		m_imgBase, 
		reinterpret_cast<const IMAGE_DATA_DIRECTORY*>(&NT_HEADER->OptionalHeader.DataDirectory),
		MEMBER(IMAGE_EXPORT_DIRECTORY, m_imgBase, static_cast<const IMAGE_DATA_DIRECTORY*>(ExportDir())->VirtualAddress));
}

__checkReturn
bool
CImage::IsForwardedExport(
	__in const void* addr
	)
{
	if ((DWORD)(static_cast<const char*>(addr) - m_imgBase) > static_cast<const IMAGE_DATA_DIRECTORY*>(ExportDir())->VirtualAddress &&
		(DWORD)(static_cast<const char*>(addr) - m_imgBase) < static_cast<const IMAGE_DATA_DIRECTORY*>(ExportDir())->VirtualAddress + static_cast<const IMAGE_DATA_DIRECTORY*>(ExportDir())->Size)
	{
		return true;
	}
	return false;
}

__checkReturn
const void* 
CImage::GetProcAddress(
	__in const char* name,
	__in const void* base,
	__in const void* imgDir,
	__in const void* exportDir
	)
{
	auto img_dir = static_cast<const IMAGE_DATA_DIRECTORY*>(imgDir);
	auto export_dir = static_cast<const IMAGE_EXPORT_DIRECTORY*>(exportDir);
	unsigned short* ordinal_table = MEMBER(unsigned short, export_dir, export_dir->AddressOfNameOrdinals - img_dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	ULONG* offset_table = MEMBER(ULONG, export_dir, export_dir->AddressOfFunctions - img_dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	ULONG* names_table = MEMBER(ULONG, export_dir, export_dir->AddressOfNames - img_dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	for (ULONG min = 0, max = export_dir->NumberOfNames, i = max >> 1, prev = max + 1;
		prev != i;
		prev = i, i = min + ((max - min) >> 1))
	{
		const char* func_name = MEMBER(const char, export_dir, names_table[i] - img_dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		int cmp = strcmp(name, func_name);

		if (cmp < 0)
			max = i;
		else if (cmp > 0)
			min = i;
		else
			return MEMBER(void, base, offset_table[ordinal_table[i]]);
	}
	return nullptr;
}

__checkReturn
const void* 
CImage::GetProcAddress(
	__in size_t ordinal,
	__in const void* base,
	__in const void* imgDir,
	__in const void* exportDir
	)
{
	auto img_dir = static_cast<const IMAGE_DATA_DIRECTORY*>(imgDir);
	auto export_dir = static_cast<const IMAGE_EXPORT_DIRECTORY*>(exportDir);
	unsigned short* ordinal_table = MEMBER(unsigned short, export_dir, export_dir->AddressOfNameOrdinals - img_dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	ULONG* offset_table = MEMBER(ULONG, export_dir, export_dir->AddressOfFunctions - img_dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		
	if (ordinal > export_dir->NumberOfFunctions)
		return nullptr;

	size_t addr = offset_table[ordinal - export_dir->Base];

	if (addr > img_dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress &&
		addr < img_dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + img_dir[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
	{
		//forwarded export						
		return nullptr;
	}

	if (addr)
		return MEMBER(void, base, addr);
	return nullptr;
}
