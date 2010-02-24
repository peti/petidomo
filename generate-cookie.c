/*
   $Source$
   $Revision$

   Copyright (C) 2001 by Peter Simons <simons@cryp.to>.

   This file is part of Petidomo.

   Petidomo is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Petidomo is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.
*/

#include "petidomo.h"
#include "md5.h"
#include <string.h>

static char* encode_digest_to_ascii(unsigned char digest[16])
    {
    int                  i;
    static const char    hex[] = "0123456789ABCDEF";
    char *               buffer;

    buffer = xmalloc(33);
    for (i = 0; i < 16; i++)
	{
	buffer[i+i] = hex[digest[i] >> 4];
	buffer[i+i+1] = hex[digest[i] & 0x0f];
	}

    buffer[i+i] = '\0';
    return buffer;
    }

char* generate_cookie(const char* buffer)
    {
    unsigned char digest[MD5_DIGEST_SIZE];
    md5_buffer(buffer, strlen(buffer), digest);
    return encode_digest_to_ascii(digest);
    }
