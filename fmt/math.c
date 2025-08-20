#include "9.h"

#define NANEXP (2047 << 20)
#define NANMASK (2047 << 20)
#define NANSIGN (1 << 31)

double Fabs(double arg)
{
    if (arg < 0)
        return -arg;
    return arg;
}

double NaN(void)
{
    FPdbleword a = {
        0,
    };

    a.hi = NANEXP;
    a.lo = 1;
    return a.x;
}

int isNaN(double d)
{
    FPdbleword a = {
        0,
    };

    a.x = d;
    if ((a.hi & NANMASK) != NANEXP)
        return 0;
    return !isInf(d, 0);
}

double Inf(int sign)
{
    FPdbleword a = {
        0,
    };

    a.hi = NANEXP;
    a.lo = 0;
    if (sign < 0)
        a.hi |= NANSIGN;
    return a.x;
}

int isInf(double d, int sign)
{
    FPdbleword a = {
        0,
    };

    a.x = d;
    if (a.lo != 0)
        return 0;
    if (a.hi == NANEXP)
        return sign >= 0;
    if (a.hi == (NANEXP | NANSIGN))
        return sign <= 0;
    return 0;
}

static double tab[] = {
    1.0e0,   1.0e1,   1.0e2,   1.0e3,   1.0e4,   1.0e5,   1.0e6,   1.0e7,   1.0e8,   1.0e9,   1.0e10,  1.0e11,  1.0e12,
    1.0e13,  1.0e14,  1.0e15,  1.0e16,  1.0e17,  1.0e18,  1.0e19,  1.0e20,  1.0e21,  1.0e22,  1.0e23,  1.0e24,  1.0e25,
    1.0e26,  1.0e27,  1.0e28,  1.0e29,  1.0e30,  1.0e31,  1.0e32,  1.0e33,  1.0e34,  1.0e35,  1.0e36,  1.0e37,  1.0e38,
    1.0e39,  1.0e40,  1.0e41,  1.0e42,  1.0e43,  1.0e44,  1.0e45,  1.0e46,  1.0e47,  1.0e48,  1.0e49,  1.0e50,  1.0e51,
    1.0e52,  1.0e53,  1.0e54,  1.0e55,  1.0e56,  1.0e57,  1.0e58,  1.0e59,  1.0e60,  1.0e61,  1.0e62,  1.0e63,  1.0e64,
    1.0e65,  1.0e66,  1.0e67,  1.0e68,  1.0e69,  1.0e70,  1.0e71,  1.0e72,  1.0e73,  1.0e74,  1.0e75,  1.0e76,  1.0e77,
    1.0e78,  1.0e79,  1.0e80,  1.0e81,  1.0e82,  1.0e83,  1.0e84,  1.0e85,  1.0e86,  1.0e87,  1.0e88,  1.0e89,  1.0e90,
    1.0e91,  1.0e92,  1.0e93,  1.0e94,  1.0e95,  1.0e96,  1.0e97,  1.0e98,  1.0e99,  1.0e100, 1.0e101, 1.0e102, 1.0e103,
    1.0e104, 1.0e105, 1.0e106, 1.0e107, 1.0e108, 1.0e109, 1.0e110, 1.0e111, 1.0e112, 1.0e113, 1.0e114, 1.0e115, 1.0e116,
    1.0e117, 1.0e118, 1.0e119, 1.0e120, 1.0e121, 1.0e122, 1.0e123, 1.0e124, 1.0e125, 1.0e126, 1.0e127, 1.0e128, 1.0e129,
    1.0e130, 1.0e131, 1.0e132, 1.0e133, 1.0e134, 1.0e135, 1.0e136, 1.0e137, 1.0e138, 1.0e139, 1.0e140, 1.0e141, 1.0e142,
    1.0e143, 1.0e144, 1.0e145, 1.0e146, 1.0e147, 1.0e148, 1.0e149, 1.0e150, 1.0e151, 1.0e152, 1.0e153, 1.0e154, 1.0e155,
    1.0e156, 1.0e157, 1.0e158, 1.0e159,
};

double Pow10(int n)
{
    int m;

    if (n < 0)
    {
        n = -n;
        if (n < sizeof(tab) / sizeof(tab[0]))
            return 1 / tab[n];
        m = n / 2;
        return 1 / (Pow10(m) * Pow10(n - m));
    }
    if (n < sizeof(tab) / sizeof(tab[0]))
        return tab[n];
    m = n / 2;
    return Pow10(m) * Pow10(n - m);
}

#define MASK 0x7ffL
#define SHIFT 20
#define BIAS 1022L
#define SIG 52

double Frexp(double d, int *ep)
{
    FPdbleword x = {
        0,
    };
    FPdbleword a = {
        0,
    };

    *ep = 0;
    /* order matters: only isNaN can operate on NaN */
    if (isNaN(d) || isInf(d, 0) || d == 0)
        return d;
    x.x = d;
    a.x = Fabs(d);
    if ((a.hi >> SHIFT) == 0)
    { /* normalize subnormal numbers */
        x.x = (double)(1ULL << SIG) * d;
        *ep = -SIG;
    }
    *ep += ((x.hi >> SHIFT) & MASK) - BIAS;
    x.hi &= ~(MASK << SHIFT);
    x.hi |= BIAS << SHIFT;
    return x.x;
}

double Ldexp(double d, int deltae)
{
    int e, bits;
    FPdbleword x = {
        0,
    };
    ulong z;

    if (d == 0)
        return 0;
    x.x = d;
    e = (x.hi >> SHIFT) & MASK;
    if (deltae >= 0 || e + deltae >= 1)
    { /* no underflow */
        e += deltae;
        if (e >= MASK)
        { /* overflow */
            if (d < 0)
                return Inf(-1);
            return Inf(1);
        }
    }
    else
    { /* underflow gracefully */
        deltae = -deltae;
        /* need to shift d right deltae */
        if (e > 1)
        { /* shift e-1 by exponent manipulation */
            deltae -= e - 1;
            e = 1;
        }
        if (deltae > 0 && e == 1)
        { /* shift 1 by switch from 1.fff to 0.1ff */
            deltae--;
            e = 0;
            x.lo >>= 1;
            x.lo |= (x.hi & 1) << 31;
            z = x.hi & ((1 << SHIFT) - 1);
            x.hi &= ~((1 << SHIFT) - 1);
            x.hi |= (1 << (SHIFT - 1)) | (z >> 1);
        }
        while (deltae > 0)
        { /* shift bits down */
            bits = deltae;
            if (bits > SHIFT)
                bits = SHIFT;
            x.lo >>= bits;
            x.lo |= (x.hi & ((1 << bits) - 1)) << (32 - bits);
            z = x.hi & ((1 << SHIFT) - 1);
            x.hi &= ~((1 << SHIFT) - 1);
            x.hi |= z >> bits;
            deltae -= bits;
        }
    }
    x.hi &= ~(MASK << SHIFT);
    x.hi |= (long)e << SHIFT;
    return x.x;
}

double Modf(double d, double *ip)
{
    FPdbleword x = {
        0,
    };
    int e;

    x.x = d;
    e = (x.hi >> SHIFT) & MASK;
    if (e == MASK)
    {
        *ip = d;
        if (x.lo != 0 || (x.hi & 0xfffffL) != 0) /* NaN */
            return d;
        /* ±Inf */
        x.hi &= 0x80000000L;
        return x.x;
    }
    if (d < 1)
    {
        if (d < 0)
        {
            x.x = Modf(-d, ip);
            *ip = -*ip;
            return -x.x;
        }
        *ip = 0;
        return d;
    }
    e -= BIAS;
    if (e <= SHIFT + 1)
    {
        x.hi &= ~(0x1fffffL >> e);
        x.lo = 0;
    }
    else if (e <= SHIFT + 33)
        x.lo &= ~(0x7fffffffL >> (e - SHIFT - 2));
    *ip = x.x;
    return d - x.x;
}