////////////////////////////////////////////////////////////////////////////////
// libtaskd - Task Server Client Library
//
// Copyright 2010 - 2011, Göteborg Bit Factory.
// Copyright 2010 - 2011, Paul Beckingham, Federico Hernandez.
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the
//
//     Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor,
//     Boston, MA
//     02110-1301
//     USA
//
////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_TASKDH
#define INCLUDED_TASKDH

#include <stdio.h>                      // For size_t

#ifdef __cplusplus
extern "C"
{
#endif

// TODO Protocol status codes as defines?

int    taskd_request     (const char*, int, const char*);
int    taskd_ssl_request (const char*, int, const char*, const char*);
size_t taskd_response    (void*, size_t);

#ifdef __cplusplus
};
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
