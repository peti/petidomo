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
*/

#ifndef __DEBUG_H__
#define __DEBUG_H__ 1

/********** Known debug modules. **********/

enum {
    DEBUG_MAIN,
    DEBUG_COMMAND,
    DEBUG_LISTSERV,
    DEBUG_HERMES,
    DEBUG_CONFIG,
    DEBUG_RFCPARSE,
    DEBUG_MAILER,
    DEBUG_ACL,
    DEBUG_AUTHEN,
    DEBUG_FILTER,
    DEBUG_ARCHIVE,
    DEBUG_EOL
};

#define MODULE_TABLE const char * const ModuleTable[] = { \
    "main", \
    "command", \
    "listserv", \
    "hermes", \
    "config", \
    "rfcparse", \
    "mailer", \
    "acl", \
    "authen", \
    "filter", \
    "archive", \
     NULL };

/********** Prototypes **********/

#ifdef DEBUG
#  ifdef __cplusplus
      extern "C" {
#  endif
extern char * debug_fname;
void setDebugLevel(unsigned short, unsigned short);
unsigned short getDebugLevel(unsigned short);
void _debug(unsigned short, unsigned short, const char *, ...);
#  ifdef __cplusplus
      }
#  endif
#  ifdef __GNUC__
#     define debug(x) { debug_fname = __FUNCTION__; _debug x; }
#  else
#     define debug(x) { debug_fname = "unknown"; _debug x; }
#  endif
#else
#  define setDebugLevel(a,b) ;
#  define debug(x) ;
#endif

#endif /* !__DEBUG_H__ */
