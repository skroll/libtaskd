#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <taskd.h>

int sync_task (
  const char* server,
  const char* certificate,
  const char* organization,
  const char* user,
  const char* password,
  const char* task,
  const char* key)
{
  taskd_authenticate (server, organization, user, password, certificate);
  taskd_add_local_change (task);

  char sync_key[40];
  strcpy (sync_key, key);
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
    return 1;
  }

  return 0;
}

void usage ()
{
  printf ("\nUsage: add [OPTIONS] 'task description'\n"
          "\n"
          "Supported options:\n"
          "  -c certificate    (default: './client.cert.pem')\n"
          "  -i uuid\n"
          "  -k key            (default: '')\n"
          "  -o organization   (default: 'Public')\n"
          "  -p password\n"
          "  -s server:port    (default: 'localhost:6544')\n"
          "  -u user           (default: 'USER')\n"
          "\n");
}

int main (int argc, char** argv)
{
  // Defaults.
  char certificate[128]  = "./client.cert.pem";
  char uuid[40]          = "";
  char key[128]          = "";
  char organization[128] = "Public";
  char password[128]     = "password";
  char server[128]       = "localhost:6544";
  char user[128]         = "USER";
  char description[128];

  // Process arguments.
  int arg;
  while ((arg = getopt (argc, argv, "c:i:hk:o:p:s:u:")) != -1)
  {
    switch (arg)
    {
    case 'c': strcpy (certificate,  optarg); break;
    case 'i': strcpy (uuid,         optarg); break;
    case 'k': strcpy (key,          optarg); break;
    case 'o': strcpy (organization, optarg); break;
    case 'p': strcpy (password,     optarg); break;
    case 's': strcpy (server,       optarg); break;
    case 'u': strcpy (user,         optarg); break;
    case 'h': usage ();                      return 1;
    }
  }

  argc -= optind;
  argv += optind;

  if (!argc)
  {
    printf ("You must specify a task description.\n");
    return 1;
  }

  strcpy (description, *argv);

  // Compose the current UTC time, in ISO format.
  time_t now = time (NULL);
  struct tm* t = gmtime (&now);
  char entry [18] = "";
  sprintf (entry, "%04d%02d%02dT%02d%02d%02dZ",
           t->tm_year + 1900,
           t->tm_mon + 1,
           t->tm_mday,
           t->tm_hour,
           t->tm_min,
           t->tm_sec);

  // Compose the task JSON.
  char task[256];
  sprintf (task, "{\"entry\":\"%s\",\"description\":\"%s\",\"uuid\":\"%s\"}",
           entry,
           description,
           uuid);

  // Sync with the server.
  return sync_task (server,
                    certificate,
                    organization,
                    user,
                    password,
                    task,
                    key);
}

