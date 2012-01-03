////////////////////////////////////////////////////////////////////////////////
// libtaskd - Task Server Client Library
//
// Copyright 2010 - 2012, Göteborg Bit Factory.
// Copyright 2010 - 2012, Paul Beckingham, Federico Hernandez.
// All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// http://www.opensource.org/licenses/mit-license.php
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
