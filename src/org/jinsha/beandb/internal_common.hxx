#pragma once

#include <stdio.h>

#define dlog(format, ...) blog_("dbg", format, __VA_ARGS__)
#define ilog(format, ...) blog_("info", format, __VA_ARGS__)
#define wlog(format, ...) blog_("warn", format, __VA_ARGS__)
#define elog(format, ...) blog_("err", format, __VA_ARGS__)
#define clog(format, ...) blog_("critical", format, __VA_ARGS__)

#define blog_(level, format, ...) { \
fprintf(stdout, "[bean][%s] ", level); fprintf(stdout,  format, __VA_ARGS__); fprintf(stdout,"\n"); }
