#include "lcthw/bstrlib.h"
#include "minunit.h"
#include <lcthw/ringbuffer.h>
#include <assert.h>
#include <string.h>

#define BUFFER_SIZE 10

static RingBuffer *buffer = NULL;

char *test_create() {
    buffer = RingBuffer_create(BUFFER_SIZE);
    mu_assert(buffer != NULL, "Failed to create the ring buffer.");
    mu_assert(buffer->length == BUFFER_SIZE + 1, "Buffer size is incorrect.");
    mu_assert(RingBuffer_available_data(buffer) == 0, "Buffer should be empty.");
    mu_assert(RingBuffer_available_space(buffer) == BUFFER_SIZE, "Buffer space calculation is incorrect.");
    return NULL;
}

char *test_destroy() {
    RingBuffer_destroy(buffer);
    return NULL;
}

char *test_write_read() {
    char *data = "hello";
    int write_size = strlen(data);
    int rc = RingBuffer_write(buffer, data, write_size);
    mu_assert(rc == write_size, "Failed to write to the buffer.");
    mu_assert(RingBuffer_available_data(buffer) == write_size, "Available data size is incorrect after write.");
    mu_assert(RingBuffer_available_space(buffer) == BUFFER_SIZE - write_size, "Available space is incorrect after write.");

    char read_data[BUFFER_SIZE] = {0};
    rc = RingBuffer_read(buffer, read_data, write_size);
    mu_assert(rc == write_size, "Failed to read from the buffer.");
    mu_assert(strcmp(data, read_data) == 0, "Data read from buffer does not match data written.");
    mu_assert(RingBuffer_available_data(buffer) == 0, "Buffer should be empty after reading.");
    mu_assert(RingBuffer_available_space(buffer) == BUFFER_SIZE, "Buffer space calculation is incorrect after reading.");
    return NULL;
}

char *test_overflow() {
    char *data = "overflowtest";
    int write_size = strlen(data);

    // Attempt to write more data than the buffer can hold
    int rc = RingBuffer_write(buffer, data, write_size);
    mu_assert(rc == -1, "Should not allow writing beyond buffer capacity.");
    mu_assert(RingBuffer_available_data(buffer) <= BUFFER_SIZE, "Buffer should not exceed capacity.");
    return NULL;
}

char *test_wraparound() {
    char *data1 = "12345";
    char *data2 = "67890";
    char read_data[BUFFER_SIZE] = {0};

    // Write and read to force wraparound
    RingBuffer_write(buffer, data1, strlen(data1));
    RingBuffer_read(buffer, read_data, strlen(data1));
    RingBuffer_write(buffer, data2, strlen(data2));

    // Verify data integrity
    RingBuffer_read(buffer, read_data, strlen(data2));
    mu_assert(strcmp(data2, read_data) == 0, "Data integrity failed during wraparound.");

    return NULL;
}

char *test_gets() {
    char *data = "testgets";
    int write_size = strlen(data);
    RingBuffer_write(buffer, data, write_size);

    bstring result = RingBuffer_gets(buffer, write_size);
    mu_assert(result != NULL, "Failed to get data from buffer.");

    bstring expected = bfromcstr(data);
    mu_assert(bstrcmp(result, expected) == 0, "Data from gets does not match expected.");
    mu_assert(RingBuffer_available_data(buffer) == 0, "Buffer should be empty after gets.");
    
    bdestroy(result);
    bdestroy(expected);

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_write_read);
    mu_run_test(test_overflow);
    mu_run_test(test_wraparound);
    mu_run_test(test_gets);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
