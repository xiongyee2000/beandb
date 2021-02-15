#pragma once

#include <stdio.h>

#include "jsoncpp/json/value.h"

#define dlog(format, ...) blog_("dbg", format, __VA_ARGS__)
#define ilog(format, ...) blog_("info", format, __VA_ARGS__)
#define wlog(format, ...) blog_("warn", format, __VA_ARGS__)
#define elog(format, ...) blog_("err", format, __VA_ARGS__)
#define clog(format, ...) blog_("critical", format, __VA_ARGS__)

#define blog_(level, format, ...) { \
fprintf(stdout, "[bean][%s] ", level); fprintf(stdout,  format, __VA_ARGS__); fprintf(stdout,"\n"); }

namespace org {
namespace jinsha {
namespace bean {

enum {
    op_eq = 0,
    op_le,
    op_lt,
    op_ge,
    op_gt,
} optype;

}
}
}