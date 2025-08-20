#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define USED(x)                                                                                                        \
    if (x)                                                                                                             \
        ;                                                                                                              \
    else
#define nelem(x) (sizeof(x) / sizeof((x)[0]))
#define nil ((void *)0)

#define IOUNIT 32768U                  /* default buffer size for 9p io */
#define STATMAX 65535U                 /* max length of machine-independent stat structure */
#define DIRMAX (sizeof(Dir) + STATMAX) /* max length of Dir structure */
#define ERRMAX 128                     /* max length of error string */

typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef signed char schar;
typedef long long vlong;
typedef unsigned long long uvlong;
typedef long intptr;
typedef unsigned long uintptr;
typedef unsigned long usize;
typedef uint Rune;
typedef union FPdbleword FPdbleword;

enum
{
    UTFmax = 4,         /* maximum bytes per rune */
    Runesync = 0x80,    /* cannot represent part of a UTF sequence (<) */
    Runeself = 0x80,    /* rune and UTF sequences are the same (<) */
    Runeerror = 0xFFFD, /* decoding error in UTF */
    Runemax = 0x10FFFF, /* 21 bit rune */
};

extern int errstr(char *, uint);
extern void rerrstr(char *, uint);
extern void werrstr(char *, ...);
extern void exits(char *);
extern int exitcode(char *);

/* FCR */
#define FPINEX (1 << 5)
#define FPUNFL ((1 << 4) | (1 << 1))
#define FPOVFL (1 << 3)
#define FPZDIV (1 << 2)
#define FPINVAL (1 << 0)
#define FPRNR (0 << 10)
#define FPRZ (3 << 10)
#define FPRPINF (2 << 10)
#define FPRNINF (1 << 10)
#define FPRMASK (3 << 10)
#define FPPEXT (3 << 8)
#define FPPSGL (0 << 8)
#define FPPDBL (2 << 8)
#define FPPMASK (3 << 8)
/* FSR */
#define FPAINEX FPINEX
#define FPAOVFL FPOVFL
#define FPAUNFL FPUNFL
#define FPAZDIV FPZDIV
#define FPAINVAL FPINVAL
union FPdbleword {
    double x;
    struct
    { /* little endian */
        ulong lo;
        ulong hi;
    };
};

/*
 * rune routines
 */
extern int runetochar(char *, Rune *);
extern int chartorune(Rune *, char *);
extern int runelen(long);
extern int runenlen(Rune *, int);
extern int fullrune(char *, int);
extern int utflen(char *);
extern int utfnlen(char *, long);
extern char *utfrune(char *, long);
extern char *utfrrune(char *, long);
extern char *utfutf(char *, char *);
extern char *utfecpy(char *, char *, char *);

extern Rune *runestrcat(Rune *, Rune *);
extern Rune *runestrchr(Rune *, Rune);
extern int runestrcmp(Rune *, Rune *);
extern Rune *runestrcpy(Rune *, Rune *);
extern Rune *runestrncpy(Rune *, Rune *, long);
extern Rune *runestrecpy(Rune *, Rune *, Rune *);
extern Rune *runestrdup(Rune *);
extern Rune *runestrncat(Rune *, Rune *, long);
extern int runestrncmp(Rune *, Rune *, long);
extern Rune *runestrrchr(Rune *, Rune);
extern long runestrlen(Rune *);
extern Rune *runestrstr(Rune *, Rune *);

extern Rune tolowerrune(Rune);
extern Rune totitlerune(Rune);
extern Rune toupperrune(Rune);
extern int isalpharune(Rune);
extern int islowerrune(Rune);
extern int isspacerune(Rune);
extern int istitlerune(Rune);
extern int isupperrune(Rune);

/*
 * print routines
 */
typedef struct Fmt Fmt;
struct Fmt
{
    uchar runes;         /* output buffer is runes or chars? */
    void *start;         /* of buffer */
    void *to;            /* current place in the buffer */
    void *stop;          /* end of the buffer; overwritten if flush fails */
    int (*flush)(Fmt *); /* called when to == stop */
    void *farg;          /* to make flush a closure */
    int nfmt;            /* num chars formatted so far */
    va_list args;        /* args passed to dofmt */
    int r;               /* % format Rune */
    int width;
    int prec;
    ulong flags;
};

enum
{
    FmtWidth = 1,
    FmtLeft = FmtWidth << 1,
    FmtPrec = FmtLeft << 1,
    FmtSharp = FmtPrec << 1,
    FmtSpace = FmtSharp << 1,
    FmtSign = FmtSpace << 1,
    FmtZero = FmtSign << 1,
    FmtUnsigned = FmtZero << 1,
    FmtShort = FmtUnsigned << 1,
    FmtLong = FmtShort << 1,
    FmtVLong = FmtLong << 1,
    FmtComma = FmtVLong << 1,
    FmtByte = FmtComma << 1,

    FmtFlag = FmtByte << 1
};

extern double NaN(void);
extern double Inf(int);
extern int isNaN(double);
extern int isInf(double, int);
extern double Pow10(int n);
extern double Frexp(double d, int *ep);
extern double Ldexp(double d, int deltae);
extern double Modf(double d, double *ip);
extern double Fabs(double arg);

extern int print(char *, ...);
extern char *seprint(char *, char *, char *, ...);
extern char *vseprint(char *, char *, char *, va_list);
extern int snprint(char *, int, char *, ...);
extern int vsnprint(char *, int, char *, va_list);
extern char *smprint(char *, ...);
extern char *vsmprint(char *, va_list);
extern int sprint(char *, char *, ...);
extern int fprint(int, char *, ...);
extern int vfprint(int, char *, va_list);

extern int runesprint(Rune *, char *, ...);
extern int runesnprint(Rune *, int, char *, ...);
extern int runevsnprint(Rune *, int, char *, va_list);
extern Rune *runeseprint(Rune *, Rune *, char *, ...);
extern Rune *runevseprint(Rune *, Rune *, char *, va_list);
extern Rune *runesmprint(char *, ...);
extern Rune *runevsmprint(char *, va_list);

extern int fmtfdinit(Fmt *, int, char *, int);
extern int fmtfdflush(Fmt *);
extern int fmtstrinit(Fmt *);
extern char *fmtstrflush(Fmt *);
extern int runefmtstrinit(Fmt *);
extern Rune *runefmtstrflush(Fmt *);

extern int fmtinstall(int, int (*)(Fmt *));
extern int dofmt(Fmt *, char *);
extern int dorfmt(Fmt *, Rune *);
extern int fmtprint(Fmt *, char *, ...);
extern int fmtvprint(Fmt *, char *, va_list);
extern int fmtrune(Fmt *, int);
extern int fmtstrcpy(Fmt *, char *);
extern int fmtrunestrcpy(Fmt *, Rune *);
extern double charstod(int (*)(void *), void *);

extern int (*doquote)(int);
/*
 * error string for %r
 * supplied on per os basis, not part of fmt library
 */
extern int errfmt(Fmt *f);

/*
 * quoted strings
 */
extern char *unquotestrdup(char *);
extern Rune *unquoterunestrdup(Rune *);
extern char *quotestrdup(char *);
extern Rune *quoterunestrdup(Rune *);
extern int quotestrfmt(Fmt *);
extern int quoterunestrfmt(Fmt *);
extern void quotefmtinstall(void);
extern int (*doquote)(int);
extern int needsrcquote(int);

/*
 *  Bio
 */
typedef struct Biobuf Biobuf;

enum
{
    Bsize = IOUNIT,
    Bungetsize = UTFmax + 1, /* space for ungetc */
    Bmagic = 0x314159,
    Beof = -1,
    Bbad = -2,

    Binactive = 0, /* states */
    Bractive,
    Bwactive,
    Bracteof,

    Bend,
};

struct Biobuf
{
    int icount;                         /* neg num of bytes at eob */
    int ocount;                         /* num of bytes at bob */
    int rdline;                         /* num of bytes after rdline */
    int runesize;                       /* num of bytes of last getrune */
    int state;                          /* r/w/inactive */
    int fid;                            /* open file */
    int flag;                           /* magic if malloc'ed */
    vlong offset;                       /* offset of buffer in file */
    int bsize;                          /* size of buffer */
    uchar *bbuf;                        /* pointer to beginning of buffer */
    uchar *ebuf;                        /* pointer to end of buffer */
    uchar *gbuf;                        /* pointer to good data in buf */
    void (*errorf)(char *);             /* called on error if not nil */
    int (*iof)(Biobuf *, void *, long); /* called to do i/o */
    void *aux;                          /* user data */
    uchar b[Bungetsize + Bsize];
};

#define OREAD O_RDONLY
#define OWRITE O_WRONLY
#define OCEXEC 0
#define ORCLOSE 0
#define OTRUNC 0

#define nil ((void *)0)

#define seek(fd, offset, whence) lseek(fd, offset, whence)
#define create(name, mode, perm) creat(name, perm)
/* Dregs, redefined as functions for backwards compatibility */
#define BGETC(bp) Bgetc(bp)
#define BPUTC(bp, c) Bputc(bp, c)
#define BOFFSET(bp) Boffset(bp)
#define BLINELEN(bp) Blinelen(bp)
#define BFILDES(bp) Bfildes(bp)

int Bbuffered(Biobuf *);
int Bfildes(Biobuf *);
int Bflush(Biobuf *);
int Bgetc(Biobuf *);
int Bgetd(Biobuf *, double *);
long Bgetrune(Biobuf *);
int Binit(Biobuf *, int, int);
int Binits(Biobuf *, int, int, uchar *, int);
int Blinelen(Biobuf *);
vlong Boffset(Biobuf *);
Biobuf *Bopen(char *, int);
Biobuf *Bfdopen(int, int);
int Bprint(Biobuf *, char *, ...);
int Bvprint(Biobuf *, char *, va_list);
int Bputc(Biobuf *, int);
int Bputrune(Biobuf *, long);
void *Brdline(Biobuf *, int);
char *Brdstr(Biobuf *, int, int);
long Bread(Biobuf *, void *, long);
vlong Bseek(Biobuf *, vlong, int);
int Bterm(Biobuf *);
int Bungetc(Biobuf *);
int Bungetrune(Biobuf *);
long Bwrite(Biobuf *, void *, long);
void Blethal(Biobuf *, void (*)(char *));
void Berror(Biobuf *, char *, ...);
void Biofn(Biobuf *, int (*)(Biobuf *, void *, long));

/*
 *  synchronization
 */
typedef struct Lock
{
    int init;
    pthread_mutex_t mutex;
    int held;
} Lock;
void lock(Lock *);
int canlock(Lock *);
void unlock(Lock *);
