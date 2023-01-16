#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

void DRV_DEBUG(char *fmt, ...)
{
    va_list args;
    vprintf("<<DBG-MSG>> ", args);
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
