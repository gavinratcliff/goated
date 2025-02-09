/* === src/utils.c === Common utilities */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <goated/utils.h>

/* === PROTOTYPES === */

/* === PUBLIC FUNCTIONS === */

void _log(const char *fmt, const char *file, int line, ...) {
    va_list args;

    va_start(args, line);

    printf("LOG %s:%d ", file, line);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

void _error(const char *fmt, const char *file, int line, ...) {
    va_list args;

    va_start(args, line);

    printf("LOG %s:%d ", file, line);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);

    exit(EXIT_FAILURE);
}

/* === PRIVATE FUNCTIONS === */
