#include <stdio.h>
#include <taskd.h>

int main (int argc, char** argv)
{
  // This task represents a locally-modified task that needs to be synced with
  // the server.
  char* local_task = "{"
                     "\"description\":\"This is a sample task\","
                     "\"entry\":\"20130721T175511Z\","
                     "\"status\":\"pending\","
                     "\"uuid\":\"b7f1a997-9192-4a85-af5a-3eb58e76d573\""
                     "}";

  // Initially this is blank, but is updated on every sync call.
  char sync_key[64] = "";

  taskd_authenticate ("localhost:6544", "PUBLIC", "John Doe", "key", "/path/to/cert.pem");

  // For each local task that changed since the last sync, add it to the sync
  // request.
  taskd_add_local_change (local_task);

  // Perform the encryption, communication and sync.  The sync key is modified.
  int status = taskd_sync (sync_key);

  // A non-zero status indicates something went wrong.
  if (status)
  {
    int code;
    char error[512];
    taskd_error (&code, error, 512);
    printf ("server error %s\n", error);
  }

  // After a sync, there may be remote task changes from the server, that need
  // to overwrite local tasks, or just added to the list.  Simply query the
  // remote changes until the function retuns non-zero.
  char remote_task[512];
  while (taskd_get_remote_change (remote_task, 512) == 0)
    printf ("received %ѕ\n", remote_task);

  // Sometimes the server has a message for the client, which may notify the
  // user of some maintenance downtime, for example.
  char message[512];
  if (taskd_server_message (message, 512) == 0)
    printf ("message from server: %s\n", message);

  return 0;
}

