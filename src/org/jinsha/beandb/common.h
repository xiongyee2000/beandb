#pragma once

#include "jsoncpp/json/value.h"

//todo:
//handle 32 bits system
//auto adpation to 32 bits system
#define SYSTEM_BITS 64

namespace org {
namespace jinsha {
namespace beandb {

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

/**
 * operation type used for search condition
 */
enum opType {
    op_eq = 0,
    op_ne,
    op_le,
    op_lt,
    op_ge,
    op_gt,
    op_like,
    op_not_like,
    op_has,
} ;

/**
 * The default page size.
 * @ref Page
 */
static const unsigned int DEFAULT_PAGE_SIZE = 8u;

}
}
}