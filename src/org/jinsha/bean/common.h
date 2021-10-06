#pragma once

#include "jsoncpp/json/value.h"

//todo:
//handle 32 bits system
//auto adpation to 32 bits system
#define SYSTEM_BITS 64

namespace org {
namespace jinsha {
namespace bean {

typedef int pidType;
#if SYSTEM_BITS == 64
typedef Json::UInt64 oidType;
typedef Json::Int64 int_t;
typedef Json::UInt64 uint_t;
// typedef unsigned long oidType;
// typedef long int_t;
// typedef unsigned long uint_t;
#else 
typedef Json::UInt oidType;
typedef Json::Int int_t;
typedef Json::UInt uint_t;
// typedef unsigned int oidType;
// typedef int int_t;
// typedef unsigned int uint_t;
#endif

}
}
}