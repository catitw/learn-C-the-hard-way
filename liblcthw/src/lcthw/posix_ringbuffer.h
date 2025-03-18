#ifndef _lcthw_POSIX_RingBuffer_h
#define _lcthw_POSIX_RingBuffer_h


#include <sys/mman.h>
#include <unistd.h>

/**
 * POSIX RingBuffer structure
 * - Uses mmap to create a virtually infinite memory-mapped region.
 */
typedef struct {
    char *buffer;  // Pointer to the memory-mapped buffer
    int length; // Total capacity of the buffer
    int start;  // Read pointer
    int end;    // Write pointer
} PosixRingBuffer;

PosixRingBuffer *PosixRingBuffer_create(int length);
void PosixRingBuffer_destroy(PosixRingBuffer *buffer);
int PosixRingBuffer_write(PosixRingBuffer *buffer, const char *data, int length);
int PosixRingBuffer_read(PosixRingBuffer *buffer, char *target, int amount);
int PosixRingBuffer_empty(PosixRingBuffer *buffer);
int PosixRingBuffer_full(PosixRingBuffer *buffer);
int PosixRingBuffer_available_data(PosixRingBuffer *buffer);
int PosixRingBuffer_available_space(PosixRingBuffer *buffer);

#endif
