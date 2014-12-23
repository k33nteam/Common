#pragma once

#include <intrin.h>
#include <wtypes.h>
#include <sal.h>

#include <SDKDDKVer.h>
#include <memory>

#define MEMBER(cast, ptr, member) reinterpret_cast<cast*>(reinterpret_cast<ULONG_PTR>(ptr) + static_cast<size_t>(member))
