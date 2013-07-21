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

/*
#include <iostream>
#include <string>
#include <stdlib.h>
*/
#include <taskd.h>

/*
extern std::string taskd_response_str;
*/
static int taskd_debug_level = 0;

/*
////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" size_t taskd_response (
  char* buffer,
  size_t size)
{
  if (taskd_response_str.size () < size)
  {
    strcpy (buffer, taskd_response_str.c_str ());
    return taskd_response_str.size ();
  }
  else
  {
    strncpy (buffer, taskd_response_str.c_str (), size);
    taskd_response_str.erase (0, size);
    return size;
  }
  return 0;
}
*/

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" int taskd_authenticate (const char*, const char*, const char*, const char*, const char*)
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" int taskd_add_local_change (const char*)
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" int taskd_sync (const char*)
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" int taskd_error (int*, const char*, size_t)
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" int taskd_get_remote_change (const char*, size_t)
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" int taskd_server_message (const char*, size_t)
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" int taskd_debug (int value)
{
  int old_value = taskd_debug_level;
  taskd_debug_level = value;
  return old_value;
}

////////////////////////////////////////////////////////////////////////////////
