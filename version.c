/*
**  version.c -- Version Information for Petidomo (syntax: C/C++)
**  [automatically generated and maintained by GNU shtool]
*/

#ifdef _VERSION_C_AS_HEADER_

#ifndef _VERSION_C_
#define _VERSION_C_

#define PETIDOMO_VERSION 0x400101

typedef struct {
    const int   v_hex;
    const char *v_short;
    const char *v_long;
    const char *v_tex;
    const char *v_gnu;
    const char *v_web;
    const char *v_sccs;
    const char *v_rcs;
} petidomo_version_t;

extern petidomo_version_t petidomo_version;

#endif /* _VERSION_C_ */

#else /* _VERSION_C_AS_HEADER_ */

#define _VERSION_C_AS_HEADER_
#include "version.c"
#undef  _VERSION_C_AS_HEADER_

petidomo_version_t petidomo_version = {
    0x400101,
    "4.0b1",
    "4.0b1 (18-Jan-2001)",
    "This is Petidomo, Version 4.0b1 (18-Jan-2001)",
    "Petidomo 4.0b1 (18-Jan-2001)",
    "Petidomo/4.0b1",
    "@(#)Petidomo 4.0b1 (18-Jan-2001)",
    "$Id$"
};

#endif /* _VERSION_C_AS_HEADER_ */

