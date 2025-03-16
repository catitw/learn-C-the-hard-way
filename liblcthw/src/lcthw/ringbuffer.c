#undef NDEBUG
#include <assert.h>
#include <lcthw/dbg.h>
#include <lcthw/ringbuffer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RingBuffer *RingBuffer_create(int length) {
  RingBuffer *buffer = NULL;

  check(length > 0, "Buffer length must be greater than 0.");

  buffer = calloc(1, sizeof(RingBuffer));
  buffer->length =
      length + 1; // +1 to handle the full/empty case
                  // - if start == end, the buffer is empty
                  // - if (end + 1) % length == start, the buffer is full
  buffer->start = 0;
  buffer->end = 0;
  buffer->buffer = calloc(buffer->length, 1);

  return buffer;

error:
  if (buffer)
    free(buffer);
  return NULL;
}

void RingBuffer_destroy(RingBuffer *buffer) {
  if (buffer) {
    free(buffer->buffer);
    free(buffer);
  }
}

int RingBuffer_write(RingBuffer *buffer, char *data, int length) {
  check(buffer != NULL, "Buffer is NULL.");
  check(data != NULL, "Data is NULL.");
  check(length > 0, "Length must be greater than 0.");

  if (RingBuffer_available_data(buffer) == 0) {
    // overflow detected, reset the buffer
    buffer->start = buffer->end = 0;
  }

  check(length <= RingBuffer_available_space(buffer),
        "Not enough space: %d request, %d available",
        RingBuffer_available_data(buffer), length);

  void *result = memcpy(RingBuffer_ends_at(buffer), data, length);
  check(result != NULL, "Failed to write data into buffer.");

  RingBuffer_commit_write(buffer, length);

  return length;
error:
  return -1;
}

int RingBuffer_read(RingBuffer *buffer, char *target, int amount) {
  check(buffer != NULL, "Buffer is NULL.");
  check(target != NULL, "Target is NULL.");
  check(amount > 0, "Amount must be greater than 0.");

  check_debug(amount <= RingBuffer_available_data(buffer),
              "Not enough in the buffer: has %d, needs %d",
              RingBuffer_available_data(buffer), amount);

  void *result = memcpy(target, RingBuffer_starts_at(buffer), amount);
  check(result != NULL, "Failed to write buffer into data.");

  RingBuffer_commit_read(buffer, amount);

  if (buffer->end == buffer->start) {
    buffer->start = buffer->end = 0;
  }

  return amount;
error:
  return -1;
}

bstring RingBuffer_gets(RingBuffer *buffer, int amount) {
  check(buffer != NULL, "Buffer is NULL.");
  check(amount > 0, "Need more than 0 for gets, you gave: %d ", amount);
  check_debug(amount <= RingBuffer_available_data(buffer),
              "Not enough in the buffer.");

  bstring result = blk2bstr(RingBuffer_starts_at(buffer), amount);
  check(result != NULL, "Failed to create gets result.");
  check(blength(result) == amount, "Wrong result length.");

  RingBuffer_commit_read(buffer, amount);
  assert(RingBuffer_available_data(buffer) >= 0 && "Error in read commit.");

  return result;
error:
  return NULL;
}
