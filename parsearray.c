/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include "petidomo.h"

struct Parse ParseArray[] = {
    { "add", AddAddress },
    { "subscribe", AddAddress },
    { "delete", DeleteAddress },
    { "unsubscribe", DeleteAddress },
    { "remove", DeleteAddress },
    { "approve", setPassword },
    { "passwd", setPassword },
    { "password", setPassword },
    { "index", GenIndex },
    { "lists", GenIndex },
    { "longindex", GenIndex },
    { "help", SendHelp },
    { "who", SendSubscriberList },
    { "members", SendSubscriberList },
    { NULL, NULL }
};
