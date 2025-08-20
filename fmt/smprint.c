#include "9.h"
#include "fmtdef.h"

char *smprint(char *fmt, ...)
{
    va_list args;
    char *p;

    va_start(args, fmt);
    p = vsmprint(fmt, args);
    va_end(args);
    // if (p != nil) setmalloctag(p, getcallerpc(&fmt));
    return p;
}
