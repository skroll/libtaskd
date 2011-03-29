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

#include <iostream>
#include <string>
#include <stdlib.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/x509v3.h>

#include <taskd.h>

// In ssl.cpp
bool taskd_init_ssl ();
bool taskd_verify_certificate (SSL*);
bool taskd_send_request (BIO*, const char*);
void taskd_read_response (BIO*);
void taskd_error (const char*);

extern std::string taskd_response_str;
extern bool taskd_debug_mode;

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" int taskd_request (
  const char* server,
  const char* certificate,
  const char* request)
{
  int status = TASKD_STATUS_ERROR;

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
                if (taskd_debug_mode)
                  std::cerr << "libtaskd: connected to server\n";

                if (taskd_send_request (bio, request))
                {
                  if (taskd_debug_mode)
                    std::cerr << "libtaskd: >>> " << request << "\n";
                  taskd_read_response (bio);

                  // TODO Extract the actual status from the JSON.
                  status = TASKD_STATUS_OK;
                  std::string::size_type p;
                  p = taskd_response_str.find ("\"status\":{\"code\":");
                  if (p != std::string::npos)
                    status = strtol (taskd_response_str.substr (p + 17, 3).c_str (), (char **)NULL, 10);
                }
                else
                {
                  status = TASKD_STATUS_ERROR;
                  if (taskd_debug_mode)
                    std::cerr << "libtaskd: send error\n";
                }

                if (taskd_debug_mode)
                  std::cerr << "libtaskd: disconnected from server\n";
              }
              else
              {
                status = TASKD_STATUS_CERT_ERROR;
                if (taskd_debug_mode)
                  std::cerr << "libtaskd: Bad server cert\n";
              }
            }
            else
            {
              status = TASKD_STATUS_CERT_ERROR;
              if (taskd_debug_mode)
                taskd_error ("libtaskd: Error getting server cert");
            }
          }
          else
          {
            status = TASKD_STATUS_SSL_ERROR;
            if (taskd_debug_mode)
              taskd_error ("libtaskd: Error connecting to remote machine");
          }

          // Clean up.
          BIO_free_all (bio);
        }
        else
        {
          status = TASKD_STATUS_ERROR;
          if (taskd_debug_mode)
            taskd_error ("libtaskd: Error creating connection BIO");
        }
      }
      else
      {
        status = TASKD_STATUS_SSL_ERROR;
        if (taskd_debug_mode)
          taskd_error ("libtaskd: Failed to verify local key");
      }

      SSL_CTX_free (ctx);
    }
  }

  return status;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" int taskd_nonssl_request (
  const char* server,
  const char* request)
{
  int status = TASKD_STATUS_ERROR;

  if (taskd_init_ssl ())
  {
    // Create a new connection.
    BIO* bio = BIO_new_connect ((char*) server);
    if (bio)
    {
      // Check for a successful connection.
      if (BIO_do_connect (bio) > 0)
      {
        if (taskd_debug_mode)
          std::cerr << "libtaskd: connected to server\n";

        if (taskd_send_request (bio, request))
        {
          if (taskd_debug_mode)
            std::cerr << "libtaskd: >>> " << request << "\n";
          taskd_read_response (bio);

          // TODO Extract the actual status from the JSON.
          status = TASKD_STATUS_OK;
          std::string::size_type p;
          p = taskd_response_str.find ("\"status\":{\"code\":");
          if (p != std::string::npos)
            status = strtol (taskd_response_str.substr (p + 17, 3).c_str (), (char **)NULL, 10);
        }
        else
        {
          status = TASKD_STATUS_ERROR;
          if (taskd_debug_mode)
            std::cerr << "libtaskd: send error\n";
        }

        if (taskd_debug_mode)
          std::cerr << "libtaskd: disconnected from server\n";
      }
      else
      {
        status = TASKD_STATUS_SSL_ERROR;
        if (taskd_debug_mode)
          taskd_error ("libtaskd: Error connecting to remote machine");
      }

      // Clean up.
      BIO_free_all (bio);
    }
    else
    {
      status = TASKD_STATUS_ERROR;
      if (taskd_debug_mode)
        taskd_error ("libtaskd: Error creating connection BIO");
    }
  }

  return status;
}

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

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE
extern "C" void taskd_debug (int value)
{
  taskd_debug_mode = (value != 0 ? true : false);
}

////////////////////////////////////////////////////////////////////////////////
