/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#ifndef MAILER_H
#define MAILER_H

#include <stdio.h>
#include "rfcparse.h"

FILE * OpenMailer(const char * envelope, const char * recipients[]);
FILE * vOpenMailer(const char * envelope, ...);
int CloseMailer(FILE * fh);
int ListMail(const char *, const char *, const struct Mail *);

#endif /* !MAILER_H */
