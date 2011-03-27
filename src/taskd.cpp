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

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/x509v3.h>

#include <taskd.h>

// In ssl.cpp
void taskd_init_ssl ();
std::string taskd_connection (const char*, int);
bool taskd_verify_certificate (SSL*);
bool taskd_send_request (BIO*, const char*);
void taskd_read_response (BIO*);
void handle_error (const char*, int, const char*);

////////////////////////////////////////////////////////////////////////////////
extern "C" int taskd_request (
  const char* server,
  int port,
  const char* certificate,
  const char* request)
{
  std::string connection = taskd_connection (server, port);
  taskd_init_ssl ();

  // Connect
  SSL_CTX* ctx = SSL_CTX_new (SSLv23_client_method ());
  if (ctx)
  {
    if (SSL_CTX_load_verify_locations (ctx, certificate, NULL))
    {
      BIO* bio = BIO_new_ssl_connect (ctx);
      if (bio)
      {
        SSL* ssl;
        BIO_get_ssl (bio, &ssl);
        SSL_set_mode (ssl, SSL_MODE_AUTO_RETRY);

        BIO_set_conn_hostname (bio, connection.c_str ());
        if (BIO_do_connect (bio) > 0)
        {
          if (SSL_get_verify_result (ssl) == X509_V_OK)
          {
            if (taskd_verify_certificate (ssl))
            {
              fprintf (stderr, "begin\n");
              if (taskd_send_request (bio, request))
                taskd_read_response (bio);
              else
                fprintf (stderr, "Error!\n");

              fprintf (stderr, "end\n");
            }
            else
              fprintf (stderr, "Bad cert\n");
          }
          else
          {
            handle_error (__FILE__, __LINE__, "Error verifying cert");
          }
        }
        else
          handle_error (__FILE__, __LINE__, "Error connecting to remote machine");

        // Clean up.
        BIO_free_all (bio);
      }
      else
        handle_error (__FILE__, __LINE__, "Error creating connection BIO");

    }
    else
      handle_error (__FILE__, __LINE__, "Failed to verify cert");

    SSL_CTX_free (ctx);
  }


  return TASKD_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////
extern "C" int taskd_nonssl_request (
  const char* server,
  int port,
  const char* request)
{
  std::string connection = taskd_connection (server, port);
  taskd_init_ssl ();

  // Create a new connection.
  BIO* bio = BIO_new_connect ((char*) connection.c_str ());
  if (bio)
  {
    // Check for a successful connection.
    if (BIO_do_connect (bio) > 0)
    {
      fprintf (stderr, "begin\n");

      if (taskd_send_request (bio, request))
        taskd_read_response (bio);
      else
        fprintf (stderr, "Error!\n");

      fprintf (stderr, "end\n");
    }
    else
      handle_error (__FILE__, __LINE__, "Error connecting to remote machine");

    // Clean up.
    BIO_free_all (bio);
  }
  else
    handle_error (__FILE__, __LINE__, "Error creating connection BIO");

  return TASKD_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////
extern "C" size_t taskd_response (
  char* buffer,
  size_t size)
{
  extern std::string response;
  if (response.size () < size)
  {
    strcpy (buffer, response.c_str ());
    return response.size ();
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
