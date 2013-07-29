#include <stdio.h>
#include <taskd.h>

int main (int argc, char** argv)
{
  // Authentication provides all the necessary credentials for communication.
  //   server        Domain/IP and port of server
  //   organization  Organization name
  //   user          User name
  //   password      Password generated by server
  //   cert          Certificate generated by server
  char* server       = "localhost:6544";
  char* organization = "Public";
  char* user         = "USER";
  char* password     = "4b821202-cf12-4965-9b1f-ab5413eb2e07";
  char* cert         = "./client.cert.pem";
  taskd_authenticate (server, organization, user, password, cert);

  // This task represents a locally-modified task that needs to be synced with
  // the server.  It is added to the list of local changes.
  char* local_task = "{"
                     "\"description\":\"This is a sample task\","
                     "\"entry\":\"20130721T175511Z\","
                     "\"status\":\"pending\","
                     "\"uuid\":\"b7f1a997-9192-4a85-af5a-3eb58e76d573\""
                     "}";
  taskd_add_local_change (local_task);

  // Perform the encryption, communication and sync.  The sync key is modified.
  // The local change list is sent and cleared.  The remote change list is
  // populated.
  char sync_key[40] = "";
  if (taskd_sync (sync_key) == 0)
  {
    // The sync key from the server should be stored and sent in the next sync
    // request.
    printf ("new sync key = %s\n", sync_key);

    // After a sync, there may be remote task changes from the server, that need
    // to overwrite local tasks, or just stored.  Simply query the remote
    // changes until the function returns non-zero.
    char remote_task[512];
    while (taskd_get_remote_change (remote_task, 512) == 0)
      printf ("received %s\n", remote_task);

    // Sometimes the server has a message for the client, which may notify the
    // user of some maintenance downtime, for example.
    char message[512];
    if (taskd_server_message (message, 512) == 0)
      printf ("message from the server: %s\n", message);
  }
  else
  {
    // When the sync fails, the error code and message are stored for retrieval.
    int code;
    char error[512];
    if (taskd_error (&code, error, 512) == 0)
      printf ("error %s\n", error);
  }

  return 0;
}

