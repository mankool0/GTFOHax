// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// IL2CPP application data

#pragma once

#include <cstdint>

// Application-specific types
#ifdef _MSC_VER
#include "../appdata/msvc/il2cpp-types.h"
#else
#include "../appdata/gcc/il2cpp-types.h"
#endif

// IL2CPP API function pointers
#ifdef _MSC_VER
#include "../appdata/msvc/il2cpp-api-functions-ptr.h"
#else
#include "../appdata/gcc/il2cpp-api-functions-ptr.h"
#endif

// IL2CPP APIs
#define DO_API(r, n, p) extern r (*n) p
#ifdef _MSC_VER
#include "../appdata/msvc/il2cpp-api-functions.h"
#else
#include "../appdata/gcc/il2cpp-api-functions.h"
#endif
#undef DO_API

// Application-specific functions
#define DO_APP_FUNC(a, r, n, p) extern r (*n) p
#define DO_APP_FUNC_METHODINFO(a, n) extern struct MethodInfo ** n
namespace app {
#ifdef _MSC_VER
	#include "../appdata/msvc/il2cpp-functions.h"
#else
	#include "../appdata/gcc/il2cpp-functions.h"
#endif
}
#undef DO_APP_FUNC
#undef DO_APP_FUNC_METHODINFO

// TypeInfo pointers
#define DO_TYPEDEF(a, n) extern n ## __Class** n ## __TypeInfo
namespace app {
#ifdef _MSC_VER
	#include "../appdata/msvc/il2cpp-types-ptr.h"
#else
	#include "../appdata/gcc/il2cpp-types-ptr.h"
#endif
}
#undef DO_TYPEDEF