#pragma once 

static const size_t SPAGE_SIZE = 0x1000;

#ifdef _WIN64
static const size_t MMUSERPROBEADDRESS = 0x000007ffffff0000;
static const size_t MMFLAGSANDINFO = 0x0000000101060001;
static const size_t MMHIGHESTUSERADDRESS = 0x000007fffffeffff;
#else
static const size_t MMUSERPROBEADDRESS = 0x7fff0000;
static const size_t MMFLAGSANDINFO = 0x80000000; //MmSystemRangeStart
static const size_t MMHIGHESTUSERADDRESS = 0x7ffeffff;
#endif
