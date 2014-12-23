#pragma once

#include <minwindef.h>

//http://undocumented.ntinternals.net/UserMode/Undocumented%20Functions/System%20Information/SYSTEM_INFORMATION_CLASS.html
enum SYSTEM_INFORMATION_CLASS_EX
{
	SystemModuleInformation = 11,
	SystemHandleInformation = 16,
	SystemSuperfetchInformation = 79,
	SystemMemoryListInformation = 80,
};

#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004

enum SUPERFETCH_INFORMATION_CLASS
{
	SuperfetchRetrieveTrace = 1, // q: CHAR[]
	SuperfetchSystemParameters, // q: PF_SYSTEM_SUPERFETCH_PARAMETERS
	SuperfetchLogEvent,
	SuperfetchGenerateTrace,
	SuperfetchPrefetch,
	SuperfetchPfnQuery, // q: PF_PFN_PRIO_REQUEST
	SuperfetchPfnSetPriority,
	SuperfetchPrivSourceQuery, // q: PF_PRIVSOURCE_QUERY_REQUEST
	SuperfetchSequenceNumberQuery, // q: ULONG
	SuperfetchScenarioPhase, // 10
	SuperfetchWorkerPriority,
	SuperfetchScenarioQuery, // q: PF_SCENARIO_PHASE_INFO
	SuperfetchScenarioPrefetch,
	SuperfetchRobustnessControl,
	SuperfetchTimeControl,
	SuperfetchMemoryListQuery, // q: PF_MEMORY_LIST_INFO
	SuperfetchMemoryRangesQuery, // q: PF_PHYSICAL_MEMORY_RANGE_INFO
	SuperfetchTracingControl,
	SuperfetchTrimWhileAgingControl,
	SuperfetchRepurposedByPrefetch, // q: PF_REPURPOSED_BY_PREFETCH_INFO // rev
	SuperfetchInformationMax
};

#define SUPERFETCH_INFORMATION_VERSION 45 // rev
#define SUPERFETCH_INFORMATION_MAGIC ('kuhC') // rev

#define MMPFNLIST_ZERO 0
#define MMPFNLIST_FREE 1
#define MMPFNLIST_STANDBY 2
#define MMPFNLIST_MODIFIED 3
#define MMPFNLIST_MODIFIEDNOWRITE 4
#define MMPFNLIST_BAD 5
#define MMPFNLIST_ACTIVE 6
#define MMPFNLIST_TRANSITION 7

#define MMPFNUSE_PROCESSPRIVATE 0
#define MMPFNUSE_FILE 1
#define MMPFNUSE_PAGEFILEMAPPED 2
#define MMPFNUSE_PAGETABLE 3
#define MMPFNUSE_PAGEDPOOL 4
#define MMPFNUSE_NONPAGEDPOOL 5
#define MMPFNUSE_SYSTEMPTE 6
#define MMPFNUSE_SESSIONPRIVATE 7
#define MMPFNUSE_METAFILE 8
#define MMPFNUSE_AWEPAGE 9
#define MMPFNUSE_DRIVERLOCKPAGE 10

#define PF_PFN_PRIO_REQUEST_VERSION 1
#define PF_PFN_PRIO_REQUEST_QUERY_MEMORY_LIST 0x1

//http://native-nt-toolkit.googlecode.com/svn/trunk/ndk/rtltypes.h

struct RTL_PROCESS_MODULE_INFORMATION
{
	unsigned int Section;
	void* MappedBase;
	void* ImageBase;
	unsigned int ImageSize;
	unsigned int Flags;
	unsigned short LoadOrderIndex;
	unsigned short InitOrderIndex;
	unsigned short LoadCount;
	unsigned short OffsetToFileName;
	char FullPathName[256];
};

struct RTL_PROCESS_MODULES
{
	unsigned int NumberOfModules;
#pragma warning(push)               
#pragma warning (disable : 4200)
	RTL_PROCESS_MODULE_INFORMATION Modules[0];
#pragma warning(pop)
};

struct SYSTEM_HANDLE
{
	ULONG ProcessId;
	BYTE ObjectTypeNumber;
	BYTE Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
};

struct SYSTEM_HANDLE_INFORMATION
{
	ULONG HandleCount;
#pragma warning(push)               
#pragma warning (disable : 4200)
	SYSTEM_HANDLE Handles[0];
#pragma warning(pop)
};

#pragma pack(push, 1)

struct SYSTEM_MEMORY_LIST_INFORMATION
{
	ULONG_PTR ZeroPageCount;
	ULONG_PTR FreePageCount;
	ULONG_PTR ModifiedPageCount;
	ULONG_PTR ModifiedNoWritePageCount;
	ULONG_PTR BadPageCount;
	ULONG_PTR PageCountByPriority[8];
	ULONG_PTR RepurposedPagesByPriority[8];
#ifdef WIN8
	ULONG_PTR ModifiedPageCountPageFile;
#endif
};

struct PAGEDIR_INFORMATION
{
	ULONGLONG UseDescription : 4; // MMPFNUSE_*
	ULONGLONG ListDescription : 3; // MMPFNLIST_*
	ULONGLONG Reserved0 : 1; // reserved for future expansion
	ULONGLONG Pinned : 1; // 1 - pinned, 0 - not pinned
	ULONGLONG PageDirectoryBase : 48; // private pages
	ULONGLONG Reserved : 7; // reserved for future expansion
};

struct MMPFN_IDENTITY
{
	PAGEDIR_INFORMATION PageDirInfo;
	size_t PageFrameIndex; // all
	void* VirtualAddress;
};

struct PF_PFN_PRIO_REQUEST
{
	ULONG Version;
	ULONG RequestFlags;
	size_t PfnCount;
	SYSTEM_MEMORY_LIST_INFORMATION MemInfo;
	MMPFN_IDENTITY PageData[0x400];
};

struct SUPERFETCH_INFORMATION
{
	ULONG Version;
	ULONG Magic;
	union
	{
		SUPERFETCH_INFORMATION_CLASS InfoClass;
		size_t Padding;
	};
	PF_PFN_PRIO_REQUEST* Data;
	size_t Length;
};

#pragma pack(pop)
