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

#define TASKD_STATUS_OK                  200
#define TASKD_STATUS_NO_CHANGE           201
#define TASKD_STATUS_DEPRECATED          300
#define TASKD_STATUS_REDIRECT            301
#define TASKD_STATUS_RETRY               302
#define TASKD_STATUS_MALFORMED           400
#define TASKD_STATUS_ENCODING            401
#define TASKD_STATUS_UNAVAILABLE         420
#define TASKD_STATUS_ACCESS              430
#define TASKD_STATUS_SUSPENDED           431
#define TASKD_STATUS_TERMINATED          432
#define TASKD_STATUS_SYNTAX_ERROR        500
#define TASKD_STATUS_PARAM_ERROR         501
#define TASKD_STATUS_UNIMPLEMENTED       502
#define TASKD_STATUS_PARAM_UNIMPLEMENTED 503
#define TASKD_STATUS_TOO_BIG             504

#define TASKD_STATUS_ERROR               600  // General Library error
#define TASKD_STATUS_SSL_ERROR           601  // SSL error
#define TASKD_STATUS_CERT_ERROR          602  // Cert error

//                               client       org          user         key
//void   taskd_authenticate     (const char*, const char*, const char*, const char*);

//void   taskd_compose_request (...);
//void   taskd_decompose_response (...);
// Warnings?
// Errors?
// Redirect?

int    taskd_request        (const char*, const char*, const char*);
int    taskd_nonssl_request (const char*, const char*);
size_t taskd_response       (char*, size_t);

void   taskd_debug          (int);

#ifdef __cplusplus
};
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
