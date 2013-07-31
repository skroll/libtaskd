////////////////////////////////////////////////////////////////////////////////
// libtaskd - Task Server Client Library
//
// Copyright 2010 - 2013, Göteborg Bit Factory.
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

// 2xx - Positive Completion reply
#define TASKD_STATUS_OK                  200
#define TASKD_STATUS_NO_CHANGE           201

// 3xx - Positive Intermediate reply
#define TASKD_STATUS_DEPRECATED          300
#define TASKD_STATUS_REDIRECT            301
#define TASKD_STATUS_RETRY               302

// 4xx - Transient Negative Completion reply
#define TASKD_STATUS_MALFORMED           400
#define TASKD_STATUS_ENCODING            401
#define TASKD_STATUS_UNAVAILABLE         420
#define TASKD_STATUS_SHUTDOWN            421
#define TASKD_STATUS_ACCESS              430
#define TASKD_STATUS_SUSPENDED           431
#define TASKD_STATUS_TERMINATED          432

// 5xx - Permanent Negative Completion reply
#define TASKD_STATUS_SYNTAX_ERROR        500
#define TASKD_STATUS_PARAM_ERROR         501
#define TASKD_STATUS_UNIMPLEMENTED       502
#define TASKD_STATUS_PARAM_UNIMPLEMENTED 503
#define TASKD_STATUS_TOO_BIG             504

int taskd_authenticate      (const char*, const char*, const char*, const char*, const char*);
int taskd_add_local_change  (const char*);
int taskd_sync              (char*);
int taskd_error             (int*, char*, size_t);
int taskd_get_remote_change (char*, size_t);
int taskd_server_message    (char*, size_t);
int taskd_limit             (int);
int taskd_debug             (int);

#ifdef __cplusplus
};
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
