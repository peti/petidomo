/*
   $Source$
   $Revision$

   Copyright (C) 2000 by CyberSolutions GmbH, Germany.

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
