/*
   $Source$
   $Revision$

   Copyright (C) 2000 by CyberSolutions GmbH, Germany.

   This file is part of OpenPetidomo.

   OpenPetidomo is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   OpenPetidomo is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with OpenPetidomo; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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
