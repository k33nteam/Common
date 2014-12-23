#pragma once

#ifdef WIN32
#define _X86_
#else
#define _AMD64_
#endif

#include <intrin.h>
#include <ntifs.h>
#include <sal.h>

#define MEMBER(cast, ptr, member) reinterpret_cast<cast*>(reinterpret_cast<ULONG_PTR>(ptr) + static_cast<size_t>(member))
