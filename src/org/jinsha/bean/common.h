#pragma once

#include <stdio.h>

#define blog_d(format, ...) blog_("dbg", format, __VA_ARGS__)
#define blog_i(format, ...) blog_("info", format, __VA_ARGS__)
#define blog_w(format, ...) blog_("warn", format, __VA_ARGS__)
#define blog_e(format, ...) blog_("err", format, __VA_ARGS__)
#define blog_c(format, ...) blog_("critical", format, __VA_ARGS__)

#define blog_(level, format, ...) { \
fprintf(stdout, "[bean][%s] ", level); fprintf(stdout,  format, __VA_ARGS__); fprintf(stdout,"\n"); }

namespace org {
namespace jinsha {
namespace bean {

typedef long otype;
typedef int ptype;
// typedef int64_t ctype;

}
}
}