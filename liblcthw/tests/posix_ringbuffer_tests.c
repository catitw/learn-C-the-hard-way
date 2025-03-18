#include "minunit.h"
#include <assert.h>
#include <lcthw/posix_ringbuffer.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 4096

static PosixRingBuffer *buffer = NULL;

char *test_create() {
  buffer = PosixRingBuffer_create(BUFFER_SIZE);
  mu_assert(buffer != NULL, "Failed to create the ring buffer.");

  // Check if the length is aligned to the page size
  int page_size = sysconf(_SC_PAGESIZE);
  int expected_length =
      ((BUFFER_SIZE + 1 + page_size - 1) / page_size) * page_size;
  mu_assert(buffer->length == expected_length,
            "Buffer length is incorrect (should be aligned to page size).");

  // Check if the buffer is initially empty
  mu_assert(PosixRingBuffer_empty(buffer), "Buffer should be empty initially.");

  // Check the available space (length - 1 due to reserved byte for full/empty
  // distinction)
  mu_assert(PosixRingBuffer_available_space(buffer) == expected_length - 1,
            "Buffer space calculation is incorrect.");

  return NULL;
}

char *test_destroy() {
  PosixRingBuffer_destroy(buffer);
  return NULL;
}

char *test_write_read() {
  char *data = "hello";
  int write_size = strlen(data);
  int rc = PosixRingBuffer_write(buffer, data, write_size);
  mu_assert(rc == write_size, "Failed to write to the buffer.");

  // Adjusted to use buffer->length instead of BUFFER_SIZE
  mu_assert(PosixRingBuffer_available_data(buffer) == write_size,
            "Available data size is incorrect after write.");
  mu_assert(PosixRingBuffer_available_space(buffer) ==
                buffer->length - write_size - 1,
            "Available space is incorrect after write.");

  char read_data[16];
  memset(read_data, 0, sizeof(read_data));
  rc = PosixRingBuffer_read(buffer, read_data, write_size);
  mu_assert(rc == write_size, "Failed to read from the buffer.");
  mu_assert(strcmp(data, read_data) == 0,
            "Data read from buffer does not match data written.");

  mu_assert(PosixRingBuffer_empty(buffer),
            "Buffer should be empty after reading.");
  mu_assert(PosixRingBuffer_available_space(buffer) == buffer->length - 1,
            "Buffer space calculation is incorrect after reading.");
  return NULL;
}

char *test_overflow() {
  char *data = malloc(buffer->length);
  mu_assert(data != NULL, "Failed to allocate memory for test data.");
  memset(data, 'A', buffer->length);

  // Attempt to write more data than the buffer can hold
  int rc = PosixRingBuffer_write(buffer, data, buffer->length);
  mu_assert(rc == buffer->length - 1,
            "Should not allow writing beyond buffer capacity.");
  mu_assert(PosixRingBuffer_available_data(buffer) == buffer->length - 1,
            "Buffer should hold maximum capacity.");

  char *temp = malloc(buffer->length);
  mu_assert(temp != NULL, "Failed to allocate memory for temp buffer.");
  PosixRingBuffer_read(buffer, temp, buffer->length - 1);
  free(temp);
  free(data);

  return NULL;
}

char *test_wraparound() {
  int half_size = (buffer->length - 1) / 2;
  mu_assert(half_size * 2 == buffer->length - 1 - 1,
            "Buffer size is not even.");

  char *data1 = malloc(half_size);
  char *data2 = malloc(half_size);
  char *read_data = malloc(buffer->length);

  mu_assert(data1 != NULL && data2 != NULL && read_data != NULL,
            "Failed to allocate memory for test data.");

  // Fill test data
  memset(data1, 'A', half_size);
  memset(data2, 'B', half_size);

  // First write: Fill half of the buffer
  int written = PosixRingBuffer_write(buffer, data1, half_size);
  mu_assert(written == half_size, "Failed to write data1.");

  // Second write: Fill most of the remaining space
  written = PosixRingBuffer_write(buffer, data2, half_size);
  mu_assert(written == half_size, "Failed to write data2.");

  // check buf is almost full
  mu_assert(PosixRingBuffer_available_space(buffer) == 1,
            "Buffer should have only 1 byte available after writes.");

  // Read the first segment of data
  memset(read_data, 0, buffer->length);
  int read = PosixRingBuffer_read(buffer, read_data, half_size);
  mu_assert(read == half_size, "Failed to read data1.");
  mu_assert(memcmp(data1, read_data, half_size) == 0,
            "Data1 mismatch after wraparound.");

  // Read the second segment of data
  memset(read_data, 0, buffer->length);
  read = PosixRingBuffer_read(buffer, read_data, half_size);
  mu_assert(read == half_size, "Failed to read data2.");
  mu_assert(memcmp(data2, read_data, half_size) == 0,
            "Data2 mismatch after wraparound.");

  // Verify that the buffer is empty after all reads
  mu_assert(PosixRingBuffer_empty(buffer),
            "Buffer should be empty after all reads.");

  free(data1);
  free(data2);
  free(read_data);
  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_create);
  mu_run_test(test_write_read);
  mu_run_test(test_overflow);
  mu_run_test(test_wraparound);
  mu_run_test(test_destroy);

  return NULL;
}

RUN_TESTS(all_tests);
