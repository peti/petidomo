/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#ifndef __RFCPARSE_H__
#define __RFCPARSE_H__ 1

struct Mail {
    char *     Header;
    char *     Body;
    char *     Envelope;
    char *     From;
    char *     Subject;
    char *     Reply_To;
    char *     Message_Id;
    char *     Approve;
    char *     ListSignature;
};

int        ParseFromLine(char *);
int        ParseReplyToLine(char *);
int        ParseMessageIdLine(char *);
int        ParseApproveLine(char *);
int        ParsePrecedenceLine(char *);
bool       isRFC822Address(const char *);
int        ParseMail(struct Mail **, char *, const char *);
char *     FindApproval(struct Mail *);
void       CanonizeAddress(char **, const char *);

#endif /* !__RFCPARSE_H__ */
