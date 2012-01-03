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

#include <iostream>
#include <string>
#include <sstream>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/x509v3.h>

////////////////////////////////////////////////////////////////////////////////
// Non-reentrant data
std::string taskd_response_str;
bool taskd_debug_mode = false;

////////////////////////////////////////////////////////////////////////////////
bool taskd_init_ssl (void)
{
  SSL_library_init ();
  SSL_load_error_strings ();
  if (taskd_debug_mode)
    std::cerr << "libtaskd: OpenSSL initialization succeeded.\n";

  return true;
}

////////////////////////////////////////////////////////////////////////////////
bool taskd_verify_certificate (SSL* ssl)
{
  X509* server_cert = SSL_get_peer_certificate (ssl);
  if (server_cert != NULL)
  {
    if (taskd_debug_mode)
      std::cerr << "libtaskd: Server certificate:\n";

    char* str = X509_NAME_oneline (X509_get_subject_name (server_cert), 0, 0);
    if (str)
    {
      if (taskd_debug_mode)
        std::cerr << "libtaskd: subject: " << str << "\n";
      free (str);
    }

    str = X509_NAME_oneline (X509_get_issuer_name (server_cert), 0, 0);
    if (str)
    {
      if (taskd_debug_mode)
        std::cerr << "libtaskd: subject: " << str << "\n";
      free (str);
    }

    // TODO How does one verify a cert?

    X509_free (server_cert);
    return true;
  }
  else
  {
    if (taskd_debug_mode)
      std::cerr << "libtaskd: No server certificate.\n";
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool taskd_send_request (BIO* bio, const char* request)
{
  int error = 0;
  for (int bytes = 0; bytes < strlen (request); bytes += error)
  {
    error = BIO_write (bio, request + bytes, strlen (request) - bytes);
    if (error <= 0)
    {
      if (! BIO_should_retry (bio))
        return false;

      error = 0;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
void taskd_read_response (BIO* bio)
{
  taskd_response_str = "";

  int error;
  int bytes = 0;
  char buf[80];

  do
  {
    error = BIO_read (bio, buf, 79);
    if (error > 0)
    {
      buf[error] = 0;

      if (taskd_debug_mode)
        std::cerr << "libtaskd: <<< " << buf << "\n";

      taskd_response_str += buf;
    }
    else
    {
      if (! BIO_should_retry (bio))
        return;

      error = 0;
    }
  }
  while (error == 0 || error == 79);
}

////////////////////////////////////////////////////////////////////////////////
void taskd_error (const char* msg)
{
  std::cerr << "libtaskd: " << msg << "\n";
  ERR_print_errors_fp (stderr);
}

////////////////////////////////////////////////////////////////////////////////
