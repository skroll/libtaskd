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

                  // Extract the actual status from the JSON.
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

          // Extract the actual status from the JSON.
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
