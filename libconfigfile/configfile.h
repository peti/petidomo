/*
 * Copyright (c) 1995-2019 Peter Simons <simons@cryp.to>
 * Copyright (c) 2000-2001 Cable & Wireless GmbH
 * Copyright (c) 1999-2000 CyberSolutions GmbH
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
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
