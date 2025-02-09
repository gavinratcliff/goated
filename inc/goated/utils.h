/* === inc/goated/utils.h === Common utilities and definitions */

#ifndef GOATED_UTILS_H
#define GOATED_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define IGNORE(_var) ((void) _var)

#define ERROR_FMT(_fmt, ...) _error(_fmt, __FILE__, __LINE__, __VA_ARGS__)
#define ERROR(_fmt) _error(_fmt, __FILE__, __LINE__)

#define LOG_FMT(_fmt, ...) _log(_fmt, __FILE__, __LINE__, __VA_ARGS__)
#define LOG(_fmt) _log(_fmt, __FILE__, __LINE__)

void _log(const char *fmt, const char *file, int line, ...);
void _error(const char *fmt, const char *file, int line, ...);

#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))
#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))

#endif /* GOATED_UTILS_H */
