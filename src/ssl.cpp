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

#include <string>
#include <sstream>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/x509v3.h>

////////////////////////////////////////////////////////////////////////////////
// Non-reentrant data
std::string response;

////////////////////////////////////////////////////////////////////////////////
void taskd_init_ssl (void)
{
  if (/*! THREAD_setup () ||*/ ! SSL_library_init ())
  {
    fprintf (stderr, "** OpenSSL initialization failed!\n");
    exit (-1);
  }

  SSL_load_error_strings ();
}

////////////////////////////////////////////////////////////////////////////////
std::string taskd_connection (const char* host, int port)
{
  std::stringstream s;
  s << host << ":" << port;
  return s.str ();
}

////////////////////////////////////////////////////////////////////////////////
bool taskd_verify_certificate (SSL* ssl)
{
  X509* server_cert = SSL_get_peer_certificate (ssl);
  if (server_cert != NULL)
  {
    fprintf (stderr, "Server certificate:\n");

    char* str = X509_NAME_oneline (X509_get_subject_name (server_cert), 0, 0);
    if (str)
    {
      fprintf (stderr, "  subject: %s\n", str);
      free (str);
    }

    str = X509_NAME_oneline (X509_get_issuer_name (server_cert), 0, 0);
    if (str)
    {
      fprintf (stderr, "  issuer: %s\n", str);
      free (str);
    }

    // TODO Check these values.

    X509_free (server_cert);
    return true;
  }
  else
    fprintf (stderr, "The SSL server does not have certificate.\n");

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
      return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
void taskd_read_response (BIO* bio)
{
  response = "";
/*
  char buf[81];
  int bytes;
  do
  {
    bytes = BIO_read (bio, buf, 80);
    if (bytes <= 0)
      return;

    buf[bytes] = '\0';
    response += buf;
  }
  while (bytes == 80);

  fprintf (stderr, "response=%s\n", response.c_str ());
*/
}

////////////////////////////////////////////////////////////////////////////////
void handle_error (const char* file, int lineno, const char* msg)
{
  fprintf (stderr, "** %s:%i %s\n", file, lineno, msg);
  ERR_print_errors_fp (stderr);
  exit (-1);
}

////////////////////////////////////////////////////////////////////////////////
