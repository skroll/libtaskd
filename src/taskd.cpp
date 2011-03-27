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
bool taskd_init_ssl ();
std::string taskd_connection (const char*, int);
bool taskd_verify_certificate (SSL*);
bool taskd_send_request (BIO*, const char*);
void taskd_read_response (BIO*);
void handle_error (const char*, int, const char*);

////////////////////////////////////////////////////////////////////////////////
extern "C" int taskd_request (
  const char* server,
  const char* certificate,
  const char* request)
{
  int status = TASKD_STATUS_OK;

  if (taskd_init_ssl ())
  {
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

          BIO_set_conn_hostname (bio, server);
          if (BIO_do_connect (bio) > 0)
          {
            if (SSL_get_verify_result (ssl) == X509_V_OK)
            {
              if (taskd_verify_certificate (ssl))
              {
                fprintf (stderr, "connected to server\n");
                if (taskd_send_request (bio, request))
                {
                  fprintf (stderr, "  >>> %s\n", request);
                  taskd_read_response (bio);
                }
                else
                  fprintf (stderr, "  send error\n");

                fprintf (stderr, "disconnected from server\n");
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
  }
  else
    status = TASKD_STATUS_ERROR;

  return status;
}

////////////////////////////////////////////////////////////////////////////////
extern "C" int taskd_nonssl_request (
  const char* server,
  const char* request)
{
  taskd_init_ssl ();

  // Create a new connection.
  BIO* bio = BIO_new_connect ((char*) server);
  if (bio)
  {
    // Check for a successful connection.
    if (BIO_do_connect (bio) > 0)
    {
      fprintf (stderr, "connected to server\n");

      if (taskd_send_request (bio, request))
      {
        fprintf (stderr, "  >>> %s\n", request);
        taskd_read_response (bio);
      }
      else
        fprintf (stderr, "  send error\n");

      fprintf (stderr, "disconnected from server\n");
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
  // TODO Dole out 'response' in chunks of size 'size'.

  extern std::string response;
  if (response.size () < size)
  {
    strcpy (buffer, response.c_str ());
    return response.size ();
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
