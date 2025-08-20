#include <errno.h>

#include "9.h"
#include "fmtdef.h"

int errfmt(Fmt *f)
{
    char buf[ERRMAX];
    char *p;
    int n;

    p = strerror(errno);
    n = strlen(p);
    memcpy(buf, p, n + 1);
    return _fmtcpy(f, buf, utflen(buf), n);
}
