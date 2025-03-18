#include <lcthw/posix_ringbuffer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Create a new PosixRingBuffer with the specified length.
 * The length is aligned to the system's page size.
 */
PosixRingBuffer *PosixRingBuffer_create(int length) {
    // Align length to the nearest page size boundary
    int page_size = sysconf(_SC_PAGESIZE);
    // one more byte to indicate full/empty, sub 1 to align to page size
    length = ((length + 1 + page_size - 1) / page_size) * page_size;

    // Allocate two consecutive memory regions
    char *buffer = mmap(NULL, length * 2, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buffer == MAP_FAILED) {
        return NULL;
    }

    // Map the first region as readable and writable
    if (mmap(buffer, length, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED | MAP_ANONYMOUS, -1, 0) == MAP_FAILED) {
        munmap(buffer, length * 2);
        return NULL;
    }

    // Map the second region to the same physical memory
    if (mmap(buffer + length, length, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED | MAP_ANONYMOUS, -1, 0) == MAP_FAILED) {
        munmap(buffer, length * 2);
        return NULL;
    }

    // Initialize the ring buffer structure
    PosixRingBuffer *ring = malloc(sizeof(PosixRingBuffer));
    if (!ring) {
        munmap(buffer, length * 2);
        return NULL;
    }

    ring->buffer = buffer;
    ring->length = length; // Includes the extra byte for alignment
    ring->start = 0;
    ring->end = 0;

    return ring;
}

/**
 * Check if the ring buffer is full.
 * The buffer is full when (end + 1) % length == start.
 */
 int PosixRingBuffer_full(PosixRingBuffer *buffer) {
    return (buffer->end + 1) % buffer->length == buffer->start;
}

/**
 * Check if the ring buffer is empty.
 * The buffer is empty when start == end.
 */
int PosixRingBuffer_empty(PosixRingBuffer *buffer) {
    return buffer->start == buffer->end;
}

/**
 * Calculate the available space in the ring buffer.
 * The available space is the total length minus the used space minus 1.
 */
int PosixRingBuffer_available_space(PosixRingBuffer *buffer) {
    return (buffer->length + buffer->start - buffer->end - 1) % buffer->length;
}

/**
 * Calculate the available data in the ring buffer.
 * The available data is the distance from end to start.
 */
int PosixRingBuffer_available_data(PosixRingBuffer *buffer) {
    return (buffer->length + buffer->end - buffer->start) % buffer->length;
}

/**
 * Write data into the ring buffer using a single memcpy.
 * Returns the number of bytes successfully written.
 */
int PosixRingBuffer_write(PosixRingBuffer *buffer, const char *data, int length) {
    int available_space = PosixRingBuffer_available_space(buffer);
    if (length > available_space) {
        length = available_space; // Truncate if data exceeds available space
    }

    // Write data in a single memcpy operation
    // The virtual memory mapping ensures that writing past the end of the buffer
    // will automatically wrap around to the beginning
    memcpy(buffer->buffer + buffer->end, data, length);

    buffer->end = (buffer->end + length) % buffer->length;

    return length;
}

/**
 * Read data from the ring buffer using a single memcpy.
 * Returns the number of bytes successfully read.
 */
int PosixRingBuffer_read(PosixRingBuffer *buffer, char *data, int length) {
    int available_data = PosixRingBuffer_available_data(buffer);
    if (length > available_data) {
        length = available_data; // Truncate if requested data exceeds available data
    }

    // Read data in a single memcpy operation
    // The virtual memory mapping ensures that reading past the end of the buffer
    // will automatically wrap around to the beginning
    memcpy(data, buffer->buffer + buffer->start, length);

    // Update the read pointer
    buffer->start = (buffer->start + length) % buffer->length;

    return length;
}

/**
 * Destroy the ring buffer and free associated resources.
 */
void PosixRingBuffer_destroy(PosixRingBuffer *buffer) {
    if (buffer) {
        munmap(buffer->buffer, buffer->length * 2);
        free(buffer);
    }
}