#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Connection *global_conn;

struct Address {
  int id;
  int set;
  char *name;
  char *email;
};

struct Database {
  int max_data;
  int max_rows;
  struct Address *rows;
};

struct Connection {
  FILE *file;
  struct Database *db;
};

void Database_close();

void die(const char *message) {
  if (errno) {
    perror(message);
  } else {
    printf("ERROR: %s\n", message);
  }

  Database_close();

  exit(1);
}

void Address_print(struct Address *addr) {
  printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load() {
  fread(&global_conn->db->max_data, sizeof(int), 1, global_conn->file);
  fread(&global_conn->db->max_rows, sizeof(int), 1, global_conn->file);

  global_conn->db->rows =
      malloc(sizeof(struct Address) * global_conn->db->max_rows);
  if (!global_conn->db->rows)
    die("Memory error");

  for (int i = 0; i < global_conn->db->max_rows; i++) {
    struct Address *addr = &global_conn->db->rows[i];
    fread(&addr->id, sizeof(int), 1, global_conn->file);
    fread(&addr->set, sizeof(int), 1, global_conn->file);

    addr->name = malloc(global_conn->db->max_data);
    addr->email = malloc(global_conn->db->max_data);

    fread(addr->name, global_conn->db->max_data, 1, global_conn->file);
    fread(addr->email, global_conn->db->max_data, 1, global_conn->file);
  }
}

void Database_open(const char *filename, char mode) {
  global_conn = malloc(sizeof(struct Connection));
  if (!global_conn)
    die("Memory error");

  global_conn->db = malloc(sizeof(struct Database));
  if (!global_conn->db)
    die("Memory error");

  if (mode == 'c') {
    global_conn->file = fopen(filename, "w");
  } else {
    global_conn->file = fopen(filename, "r+");

    if (global_conn->file) {
      Database_load();
    }
  }

  if (!global_conn->file)
    die("Failed to open the file");
}

void Database_close() {
  if (global_conn) {
    if (global_conn->file)
      fclose(global_conn->file);
    if (global_conn->db) {
      if (global_conn->db->rows) {
        for (int i = 0; i < global_conn->db->max_rows; i++) {
          free(global_conn->db->rows[i].name);
          free(global_conn->db->rows[i].email);
        }
        free(global_conn->db->rows);
      }
      free(global_conn->db);
    }
    free(global_conn);
  }
}

void Database_write() {
  rewind(global_conn->file);

  fwrite(&global_conn->db->max_data, sizeof(int), 1, global_conn->file);
  fwrite(&global_conn->db->max_rows, sizeof(int), 1, global_conn->file);

  for (int i = 0; i < global_conn->db->max_rows; i++) {
    struct Address *addr = &global_conn->db->rows[i];
    fwrite(&addr->id, sizeof(int), 1, global_conn->file);
    fwrite(&addr->set, sizeof(int), 1, global_conn->file);
    fwrite(addr->name, global_conn->db->max_data, 1, global_conn->file);
    fwrite(addr->email, global_conn->db->max_data, 1, global_conn->file);
  }

  fflush(global_conn->file);
}

void Database_create(int max_data, int max_rows) {
  global_conn->db->max_data = max_data;
  global_conn->db->max_rows = max_rows;

  global_conn->db->rows = malloc(sizeof(struct Address) * max_rows);
  if (!global_conn->db->rows)
    die("Memory error");

  for (int i = 0; i < max_rows; i++) {
    struct Address addr = {.id = i, .set = 0};
    addr.name = malloc(max_data);
    addr.email = malloc(max_data);
    if (!addr.name || !addr.email)
      die("Memory error");

    global_conn->db->rows[i] = addr;
  }
}

void Database_set(int id, const char *name, const char *email) {
  struct Address *addr = &global_conn->db->rows[id];
  if (addr->set)
    die("Already set, delete it first");

  addr->set = 1;
  char *res = strncpy(addr->name, name, global_conn->db->max_data);
  if (!res)
    die("Name copy failed");
  else
    res[global_conn->db->max_data - 1] = '\0';

  res = strncpy(addr->email, email, global_conn->db->max_data - 1);
  if (!res)
    die("Email copy failed");
  else
    res[global_conn->db->max_data - 1] = '\0';
}

void Database_get(int id) {
  struct Address *addr = &global_conn->db->rows[id];

  if (addr->set) {
    Address_print(addr);
  } else {
    die("ID is not set");
  }
}

void Database_delete(int id) {
  struct Address addr = {.id = id, .set = 0};
  addr.name = malloc(global_conn->db->max_data);
  addr.email = malloc(global_conn->db->max_data);
  if (!addr.name || !addr.email)
    die("Memory error");

  free(global_conn->db->rows[id].name);
  free(global_conn->db->rows[id].email);

  global_conn->db->rows[id] = addr;
}

void Database_list() {
  for (int i = 0; i < global_conn->db->max_rows; i++) {
    struct Address *cur = &global_conn->db->rows[i];

    if (cur->set) {
      Address_print(cur);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3)
    die("USAGE: ex17 <dbfile> <action> [action params]");

  char *filename = argv[1];
  char action = argv[2][0];

  Database_open(filename, action);

  int id = 0;

  if (argc > 3)
    id = atoi(argv[3]);

  switch (action) {
  case 'c':
    if (argc != 5)
      die("Need max_data and max_rows to create");

    int max_data = atoi(argv[3]);
    int max_rows = atoi(argv[4]);
    Database_create(max_data, max_rows);
    Database_write();
    break;

  case 'g':
    if (argc != 4)
      die("Need an id to get");

    Database_get(id);
    break;

  case 's':
    if (argc != 6)
      die("Need id, name, email to set");

    Database_set(id, argv[4], argv[5]);
    Database_write();
    break;

  case 'd':
    if (argc != 4)
      die("Need id to delete");

    Database_delete(id);
    Database_write();
    break;

  case 'l':
    Database_list();
    break;

  default:
    die("Invalid action, only: c=create, g=get, s=set, d=del, l=list");
  }

  Database_close();

  return 0;
}
