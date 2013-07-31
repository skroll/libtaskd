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

#include <queue>
#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <TLSClient.h>
#include <Msg.h>
#include <text.h>
#include <taskd.h>

// Outgoing.
static int _debug_level            = 0;
static int _limit                  = (1024*1024);
static std::string _host           = "localhost";
static std::string _port           = "6544";
static std::string _org            = "";
static std::string _user           = "";
static std::string _key            = "";
static std::string _cert           = "";
static std::deque <std::string> _local_tasks;

// Incoming.
static std::deque <std::string> _remote_tasks;
static int _remote_code            = 0;
static std::string _remote_error   = "";
static std::string _remote_message = "";

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
//
// Return status:
//   0 success
//   3 bad argument
//   4 malformed data
extern "C" int taskd_authenticate (
  const char* server,
  const char* org,
  const char* user,
  const char* key,
  const char* cert)
{
  // Validation.
  if (!server || !org || !user || !key || !cert)
    return 3;

  // Server must contain a port.
  if (strchr (server, ':') == NULL)
    return 4;

  // Cert must be an extant, readable file.
  if (access (cert, F_OK | R_OK))
    return 4;

  // A key is a UUID, and therefore either 36 characters or blank.
  if (strlen (key) != 0 && strlen (key) != 36)
    return 4;

  std::string hostPort = server;
  std::string::size_type colon = hostPort.find (':');
  _host = hostPort.substr (0, colon);
  _port = hostPort.substr (colon + 1);

  _org    = org;
  _user   = user;
  _key    = key;
  _cert   = cert;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
//
// Return status:
//   0 success
//   3 bad argument
//   4 malformed data
extern "C" int taskd_add_local_change (
  const char* task)
{
  // Validation.
  if (!task)
    return 3;

  // Task should be JSON.
  if (!strchr (task, '{') || !strchr (task, '}'))
    return 4;

  // A valid task contains a uuid, description and entry date.
  if (!strstr (task, "\"description\":") ||
      !strstr (task, "\"uuid\":")        ||
      !strstr (task, "\"entry\":"))
    return 4;

  // Store the task for the upcoming sync.
  _local_tasks.push_back (trim (task, " \n"));
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
//
// Return status:
//   0 success
//   1 no data
//   2 buffer too small
//   3 bad argument
//   4 malformed data
//   5 error
extern "C" int taskd_sync (
  char* sync_key)
{
  // Validation.
  if (!sync_key)
    return 3;

  try
  {
    // Combine local tasks into the payload.
    std::string payload = "";
    std::deque <std::string>::iterator i;
    for (i = _local_tasks.begin (); i != _local_tasks.end (); ++i)
      payload += *i + "\n";

    // Compose the sync request.
    Msg sync;
    sync.set ("protocol", "v1");
    sync.set ("type",     "sync");
    sync.set ("org",      _org);
    sync.set ("user",     _user);
    sync.set ("key",      _key);
    sync.setPayload (payload);

    // Send sync request and payload.
    TLSClient client;
    client.debug (_debug_level);
    client.limit (_limit);
    client.init (_cert);
    client.connect (_host, _port);
    client.send (sync.serialize ());

    // Get server response.
    std::string response;
    client.recv (response);
    client.bye ();

    // Parse payload.
    Msg remotes;
    remotes.parse (response);
    std::vector <std::string> tasks;
    split (tasks, remotes.getPayload (), '\n');

    // Store remote tasks and sync key.
    _remote_tasks.clear ();
    std::vector <std::string>::iterator r;
    for (r = tasks.begin (); r != tasks.end (); ++r)
    {
      if (r->size () > 1)
      {
        if ((*r)[0] == '{')
          _remote_tasks.push_back (*r);
        else
          strcpy (sync_key, r->c_str ());
      }
    }

    // Store message, if any.
    _remote_message = remotes.get ("message");

    // Store code and error.
    _remote_code  = (int) strtol (remotes.get ("code").c_str (), NULL, 10);
    _remote_error = remotes.get ("error");
  }

  catch (const std::string& e)
  {
    _remote_error = e;
    _remote_code = 500;
    return 5;
  }

  catch (...)
  {
    _remote_error = "Unknown error.";
    _remote_code = 500;
    return 5;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
//
// Return status:
//   0 success
//   1 no data
//   2 buffer too small
//   3 bad argument
extern "C" int taskd_error (
  int* code,
  char* error,
  size_t size)
{
  // Validation.
  if (!code || !error || !size)
    return 3;

  if (_remote_error != "")
  {
    if (_remote_error.size () >= size)
      return 2;

    if (!code)
      return 3;

    *code = _remote_code;
    strcpy (error, _remote_error.c_str ());
    return 0;
  }

  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
//
// Return status:
//   0 success
//   1 no data
//   2 buffer too small
extern "C" int taskd_get_remote_change (
  char* task,
  size_t size)
{
  // Validation.
  if (!task || !size)
    return 3;

  if (_remote_tasks.size ())
  {
    if (_remote_tasks[0].size () >= size)
      return 2;

    strcpy (task, _remote_tasks[0].c_str ());
    _remote_tasks.pop_front ();
    return 0;
  }

  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
//
// Return status:
//   0 success
//   1 no data
//   2 buffer too small
//   3 bad argument
extern "C" int taskd_server_message (
  char* message,
  size_t size)
{
  // Validation.
  if (!message || !size)
    return 3;

  if (_remote_message != "")
  {
    if (_remote_message.size () >= size)
      return 2;

    strcpy (message, _remote_message.c_str ());
    return 0;
  }

  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
//
// Return status:
//   Previous limit
extern "C" int taskd_limit (
  int limit)
{
  int old_limit = _limit;
  _limit = limit;
  return old_limit;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
//
// Return status:
//   Previous debug level, default 0
extern "C" int taskd_debug (
  int level)
{
  int old_level = _debug_level;
  _debug_level = level;
  return old_level;
}

////////////////////////////////////////////////////////////////////////////////
