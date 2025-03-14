#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void Database_close(struct Connection *conn);

void die(const char *message, struct Connection *conn) {
  if (errno) {
    perror(message);
  } else {
    printf("ERROR: %s\n", message);
  }

  Database_close(conn);

  exit(1);
}

void Address_print(struct Address *addr) {
  printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn) {
  fread(&conn->db->max_data, sizeof(int), 1, conn->file);
  fread(&conn->db->max_rows, sizeof(int), 1, conn->file);

  conn->db->rows = malloc(sizeof(struct Address) * conn->db->max_rows);
  if (!conn->db->rows)
    die("Memory error", conn);

  for (int i = 0; i < conn->db->max_rows; i++) {
    struct Address *addr = &conn->db->rows[i];
    fread(&addr->id, sizeof(int), 1, conn->file);
    fread(&addr->set, sizeof(int), 1, conn->file);

    addr->name = malloc(conn->db->max_data);
    addr->email = malloc(conn->db->max_data);

    fread(addr->name, conn->db->max_data, 1, conn->file);
    fread(addr->email, conn->db->max_data, 1, conn->file);
  }
}

struct Connection *Database_open(const char *filename, char mode) {
  struct Connection *conn = malloc(sizeof(struct Connection));
  if (!conn)
    die("Memory error", conn);

  conn->db = malloc(sizeof(struct Database));
  if (!conn->db)
    die("Memory error", conn);

  if (mode == 'c') {
    conn->file = fopen(filename, "w");
  } else {
    conn->file = fopen(filename, "r+");

    if (conn->file) {
      Database_load(conn);
    }
  }

  if (!conn->file)
    die("Failed to open the file", conn);

  return conn;
}

void Database_close(struct Connection *conn) {
  if (conn) {
    if (conn->file)
      fclose(conn->file);
    if (conn->db) {
      if (conn->db->rows) {
        for (int i = 0; i < conn->db->max_rows; i++) {
          free(conn->db->rows[i].name);
          free(conn->db->rows[i].email);
        }
        free(conn->db->rows);
      }
      free(conn->db);
    }
    free(conn);
  }
}

void Database_write(struct Connection *conn) {
  rewind(conn->file);

  fwrite(&conn->db->max_data, sizeof(int), 1, conn->file);
  fwrite(&conn->db->max_rows, sizeof(int), 1, conn->file);

  for (int i = 0; i < conn->db->max_rows; i++) {
    struct Address *addr = &conn->db->rows[i];
    fwrite(&addr->id, sizeof(int), 1, conn->file);
    fwrite(&addr->set, sizeof(int), 1, conn->file);
    fwrite(addr->name, conn->db->max_data, 1, conn->file);
    fwrite(addr->email, conn->db->max_data, 1, conn->file);
  }

  fflush(conn->file);
}

void Database_create(struct Connection *conn, int max_data, int max_rows) {
  conn->db->max_data = max_data;
  conn->db->max_rows = max_rows;

  conn->db->rows = malloc(sizeof(struct Address) * max_rows);
  if (!conn->db->rows)
    die("Memory error", conn);

  for (int i = 0; i < max_rows; i++) {
    struct Address addr = {.id = i, .set = 0};
    addr.name = malloc(max_data);
    addr.email = malloc(max_data);
    if (!addr.name || !addr.email)
      die("Memory error", conn);

    conn->db->rows[i] = addr;
  }
}

void Database_set(struct Connection *conn, int id, const char *name,
                  const char *email) {
  struct Address *addr = &conn->db->rows[id];
  if (addr->set)
    die("Already set, delete it first", conn);

  addr->set = 1;
  char *res = strncpy(addr->name, name, conn->db->max_data);
  if (!res)
    die("Name copy failed", conn);
  else
    res[conn->db->max_data - 1] = '\0';

  res = strncpy(addr->email, email, conn->db->max_data - 1);
  if (!res)
    die("Email copy failed", conn);
  else
    res[conn->db->max_data - 1] = '\0';
}

void Database_get(struct Connection *conn, int id) {
  struct Address *addr = &conn->db->rows[id];

  if (addr->set) {
    Address_print(addr);
  } else {
    die("ID is not set", conn);
  }
}

void Database_delete(struct Connection *conn, int id) {
  struct Address addr = {.id = id, .set = 0};
  addr.name = malloc(conn->db->max_data);
  addr.email = malloc(conn->db->max_data);
  if (!addr.name || !addr.email)
    die("Memory error", conn);

  free(conn->db->rows[id].name);
  free(conn->db->rows[id].email);

  conn->db->rows[id] = addr;
}

void Database_list(struct Connection *conn) {
  for (int i = 0; i < conn->db->max_rows; i++) {
    struct Address *cur = &conn->db->rows[i];

    if (cur->set) {
      Address_print(cur);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3)
    die("USAGE: ex17 <dbfile> <action> [action params]", NULL);

  char *filename = argv[1];
  char action = argv[2][0];
  struct Connection *conn = Database_open(filename, action);

  int id = 0;

  if (argc > 3)
    id = atoi(argv[3]);

  switch (action) {
  case 'c':
    if (argc != 5)
      die("Need max_data and max_rows to create", NULL);

    int max_data = atoi(argv[3]);
    int max_rows = atoi(argv[4]);
    Database_create(conn, max_data, max_rows);
    Database_write(conn);
    break;

  case 'g':
    if (argc != 4)
      die("Need an id to get", conn);

    Database_get(conn, id);
    break;

  case 's':
    if (argc != 6)
      die("Need id, name, email to set", conn);

    Database_set(conn, id, argv[4], argv[5]);
    Database_write(conn);
    break;

  case 'd':
    if (argc != 4)
      die("Need id to delete", conn);

    Database_delete(conn, id);
    Database_write(conn);
    break;

  case 'l':
    Database_list(conn);
    break;

  default:
    die("Invalid action, only: c=create, g=get, s=set, d=del, l=list", conn);
  }

  Database_close(conn);

  return 0;
}
