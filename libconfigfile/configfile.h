/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#ifndef __LIB_CONFIGFILE_H__
#define __LIB_CONFIGFILE_H__ 1

#include <stdlib.h>
#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

/********** Prototypes **********/

struct ConfigFile {
    char *     keyword;
    int        type;
    void *     data;
};
enum {
    CF_STRING,
    CF_INTEGER,
    CF_YES_NO,
    CF_MULTI
};

int ReadConfig(const char * filename, struct ConfigFile cf[]);
void FreeConfig(const char *);
void FreeAllConfigs(void);
char * GetConfig(const char * filename, char * keyword);
int SetConfig(const char * filename, char * keyword, const char * data);

#endif /* !defined(__LIB_CONFIGFILE_H__) */
