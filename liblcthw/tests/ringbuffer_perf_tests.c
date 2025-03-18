#include "minunit.h"
#include <assert.h>
#include <lcthw/dbg.h>
#include <lcthw/posix_ringbuffer.h>
#include <lcthw/ringbuffer.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define TEST_BUFFER_SIZE 4096 * 10240
#define NUM_OPERATIONS 1000000       // operations
#define MAX_CHUNK_SIZE 8192          // Maximum chunk size 8KB

// Use 64-bit integers to avoid overflow
typedef struct {
    int64_t bytes_written;
    int64_t bytes_read;
    double elapsed_time;
} TestResult;

// Generate identical random operations for both tests
typedef struct {
    int is_write;       // 1 for write, 0 for read
    int chunk_size;     // Size of data to read/write
} Operation;

// Generate test operations once
Operation *generate_test_operations(int count, unsigned int seed) {
    Operation *ops = malloc(count * sizeof(Operation));
    assert(ops != NULL);
    
    srand(seed);
    for (int i = 0; i < count; i++) {
        ops[i].is_write = (rand() % 2 == 0) ? 1 : 0;
        ops[i].chunk_size = rand() % MAX_CHUNK_SIZE + 1;
    }
    
    return ops;
}

// Helper function to generate random data
void generate_random_data(char *buffer, int size, unsigned int *seed) {
    for (int i = 0; i < size; i++) {
        buffer[i] = (char)(rand_r(seed) % 256);
    }
}

// Get current time in microseconds
uint64_t get_time_usec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

// Test the performance of the plain RingBuffer
TestResult test_plain_ringbuffer_performance(Operation *ops, int num_ops, unsigned int data_seed) {
    printf("Testing Plain RingBuffer Performance...\n");
    
    TestResult result = {0, 0, 0};
    RingBuffer *buffer = RingBuffer_create(TEST_BUFFER_SIZE);
    assert(buffer != NULL);
    
    char *write_data = malloc(MAX_CHUNK_SIZE);
    char *read_data = malloc(MAX_CHUNK_SIZE);
    assert(write_data != NULL && read_data != NULL);
    
    unsigned int seed = data_seed;
    uint64_t start_time = get_time_usec();
    
    for (int i = 0; i < num_ops; i++) {
        if (ops[i].is_write) {
            // Write operation
            int chunk_size = ops[i].chunk_size;
            int available = RingBuffer_available_space(buffer);
            
            if (available <= 0) continue;
            if (chunk_size > available) chunk_size = available;
            
            generate_random_data(write_data, chunk_size, &seed);
            int written = RingBuffer_write(buffer, write_data, chunk_size);
            
            if (written > 0) {
                result.bytes_written += written;
            }
        } else {
            // Read operation
            int chunk_size = ops[i].chunk_size;
            int available = RingBuffer_available_data(buffer);
            
            if (available <= 0) continue;
            if (chunk_size > available) chunk_size = available;
            
            int read = RingBuffer_read(buffer, read_data, chunk_size);
            
            if (read > 0) {
                result.bytes_read += read;
            }
        }
    }
    
    uint64_t end_time = get_time_usec();
    result.elapsed_time = (end_time - start_time) / 1000000.0;
    
    printf("Plain RingBuffer Results:\n");
    printf("  Total operations: %d\n", num_ops);
    printf("  Total bytes written: %lld\n", (long long)result.bytes_written);
    printf("  Total bytes read: %lld\n", (long long)result.bytes_read);
    printf("  Elapsed time: %.3f seconds\n", result.elapsed_time);
    printf("  Throughput: %.2f MB/s\n", 
           ((double)result.bytes_written + (double)result.bytes_read) / 
           (1024.0 * 1024.0 * result.elapsed_time));
    
    free(write_data);
    free(read_data);
    RingBuffer_destroy(buffer);
    
    return result;
}

// Test the performance of the PosixRingBuffer
TestResult test_posix_ringbuffer_performance(Operation *ops, int num_ops, unsigned int data_seed) {
    printf("\nTesting PosixRingBuffer Performance...\n");
    
    TestResult result = {0, 0, 0};
    PosixRingBuffer *buffer = PosixRingBuffer_create(TEST_BUFFER_SIZE);
    assert(buffer != NULL);
    
    char *write_data = malloc(MAX_CHUNK_SIZE);
    char *read_data = malloc(MAX_CHUNK_SIZE);
    assert(write_data != NULL && read_data != NULL);
    
    unsigned int seed = data_seed;
    uint64_t start_time = get_time_usec();
    
    for (int i = 0; i < num_ops; i++) {
        if (ops[i].is_write) {
            // Write operation
            int chunk_size = ops[i].chunk_size;
            int available = PosixRingBuffer_available_space(buffer);
            
            if (available <= 0) continue;
            if (chunk_size > available) chunk_size = available;
            
            generate_random_data(write_data, chunk_size, &seed);
            int written = PosixRingBuffer_write(buffer, write_data, chunk_size);
            
            if (written > 0) {
                result.bytes_written += written;
            }
        } else {
            // Read operation
            int chunk_size = ops[i].chunk_size;
            int available = PosixRingBuffer_available_data(buffer);
            
            if (available <= 0) continue;
            if (chunk_size > available) chunk_size = available;
            
            int read = PosixRingBuffer_read(buffer, read_data, chunk_size);
            
            if (read > 0) {
                result.bytes_read += read;
            }
        }
    }
    
    uint64_t end_time = get_time_usec();
    result.elapsed_time = (end_time - start_time) / 1000000.0;
    
    printf("PosixRingBuffer Results:\n");
    printf("  Total operations: %d\n", num_ops);
    printf("  Total bytes written: %lld\n", (long long)result.bytes_written);
    printf("  Total bytes read: %lld\n", (long long)result.bytes_read);
    printf("  Elapsed time: %.3f seconds\n", result.elapsed_time);
    printf("  Throughput: %.2f MB/s\n", 
           ((double)result.bytes_written + (double)result.bytes_read) / 
           (1024.0 * 1024.0 * result.elapsed_time));
    
    free(write_data);
    free(read_data);
    PosixRingBuffer_destroy(buffer);
    
    return result;
}

char *test_ringbuffer_consistency() {
    printf("\nRunning consistency test between implementations...\n");
  
    // Create both types of buffers
    // Note: We'll use a BIG size to make boundary crossing more likely
    const int buffer_size = 4096 * 64;
    RingBuffer *plain = RingBuffer_create(buffer_size);
    PosixRingBuffer *posix = PosixRingBuffer_create(buffer_size);
    assert(plain != NULL && posix != NULL);
  
    // Prepare test data
    char *write_data = malloc(MAX_CHUNK_SIZE);
    char *read_data_plain = malloc(MAX_CHUNK_SIZE);
    char *read_data_posix = malloc(MAX_CHUNK_SIZE);
    assert(write_data != NULL && read_data_plain != NULL &&
           read_data_posix != NULL);
  
    // Initialize random number generator
    unsigned int seed = time(NULL);
    srand(seed);
  
    int plain_total_written = 0;
    int posix_total_written = 0;
    int plain_total_read = 0;
    int posix_total_read = 0;
  
    // Perform a series of identical operations and ensure consistent results
    for (int i = 0; i < 10000; i++) {
      // Generate random data
      int chunk_size =
          rand() % 50 + 1; // Smaller chunks to avoid large differences
      generate_random_data(write_data, chunk_size, &seed);
  
      // Decide whether to read or write (60% write, 40% read)
      if (rand() % 100 < 60) {
        // Write operation
        // Get available space in each buffer
        int plain_space = RingBuffer_available_space(plain);
        int posix_space = PosixRingBuffer_available_space(posix);
  
        // Determine write size for each buffer separately
        int plain_write_size =
            chunk_size > plain_space ? plain_space : chunk_size;
        int posix_write_size =
            chunk_size > posix_space ? posix_space : chunk_size;
  
        // Only proceed if both buffers have space
        if (plain_write_size > 0 && posix_write_size > 0) {
          // Use the minimum size to ensure we write the same amount to both
          int write_size = (plain_write_size < posix_write_size)
                               ? plain_write_size
                               : posix_write_size;
  
          int plain_written = RingBuffer_write(plain, write_data, write_size);
          int posix_written =
              PosixRingBuffer_write(posix, write_data, write_size);
  
          // Check that the write operations succeeded as expected
          mu_assert(plain_written == write_size,
                    "Plain buffer wrote unexpected amount");
          mu_assert(posix_written == write_size,
                    "Posix buffer wrote unexpected amount");
  
          plain_total_written += plain_written;
          posix_total_written += posix_written;
        }
      } else {
        // Read operation
        // Get available data in each buffer
        int plain_data = RingBuffer_available_data(plain);
        int posix_data = PosixRingBuffer_available_data(posix);
  
        // Only proceed if both buffers have data
        if (plain_data > 0 && posix_data > 0) {
          // Use the minimum available data to ensure 
          // we read the same amount from both
          int read_size = (plain_data < posix_data) ? plain_data : posix_data;
          read_size = (read_size < chunk_size) ? read_size : chunk_size;
  
          int plain_read = RingBuffer_read(plain, read_data_plain, read_size);
          int posix_read =
              PosixRingBuffer_read(posix, read_data_posix, read_size);
  
          // Check that the read operations succeeded as expected
          mu_assert(plain_read == read_size,
                    "Plain buffer read unexpected amount");
          mu_assert(posix_read == read_size,
                    "Posix buffer read unexpected amount");
  
          // Ensure the data read is identical
          mu_assert(memcmp(read_data_plain, read_data_posix, read_size) == 0,
                    "Data mismatch between implementations");
  
          plain_total_read += plain_read;
          posix_total_read += posix_read;
        }
      }
    }
  
    printf("Consistency test results:\n");
    printf("  Plain buffer: wrote %d bytes, read %d bytes\n", plain_total_written,
           plain_total_read);
    printf("  Posix buffer: wrote %d bytes, read %d bytes\n", posix_total_written,
           posix_total_read);
  
    free(write_data);
    free(read_data_plain);
    free(read_data_posix);
    RingBuffer_destroy(plain);
    PosixRingBuffer_destroy(posix);
  
    return NULL;
  }

char *all_tests() {
    mu_suite_start();
    
    // First run the consistency test
    mu_run_test(test_ringbuffer_consistency);
    
    // Generate operations for performance tests
    unsigned int seed = time(NULL);
    printf("\nUsing random seed: %u\n", seed);
    
    unsigned int data_seed = seed;
    Operation *ops = generate_test_operations(NUM_OPERATIONS, seed);
    
    // Run performance tests with identical operations
    TestResult plain_result = test_plain_ringbuffer_performance(ops, NUM_OPERATIONS, data_seed);
    TestResult posix_result = test_posix_ringbuffer_performance(ops, NUM_OPERATIONS, data_seed);
    
    // Compare results
    printf("\nPerformance Comparison:\n");
    printf("  Plain RingBuffer: %.2f MB/s\n", 
           ((double)plain_result.bytes_written + (double)plain_result.bytes_read) / 
           (1024.0 * 1024.0 * plain_result.elapsed_time));
    printf("  Posix RingBuffer: %.2f MB/s\n", 
           ((double)posix_result.bytes_written + (double)posix_result.bytes_read) / 
           (1024.0 * 1024.0 * posix_result.elapsed_time));
    printf("  Speedup: %.2fx\n", 
           ((double)posix_result.bytes_written + (double)posix_result.bytes_read) * plain_result.elapsed_time / 
           (((double)plain_result.bytes_written + (double)plain_result.bytes_read) * posix_result.elapsed_time));
    
    free(ops);
    return NULL;
}

RUN_TESTS(all_tests);


// Consistency test results:
//   Plain buffer: wrote 151143 bytes, read 102491 bytes
//   Posix buffer: wrote 151143 bytes, read 102491 bytes

// Using random seed: 1742305647
// Testing Plain RingBuffer Performance...
// Plain RingBuffer Results:
//   Total operations: 1000000
//   Total bytes written: 133667395
//   Total bytes read: 133282451
//   Elapsed time: 0.804 seconds
//   Throughput: 316.71 MB/s
// 读写量明显要低很多，应该是由于该RingBuffer的实现中，如果检查到空闲空间不足会直接返回错误，而不是写入尽可能多的数据。

// Testing PosixRingBuffer Performance...
// PosixRingBuffer Results:
//   Total operations: 1000000
//   Total bytes written: 2048198886
//   Total bytes read: 2043543178
//   Elapsed time: 10.831 seconds
//   Throughput: 360.29 MB/s

// Performance Comparison:
//   Plain RingBuffer: 316.71 MB/s
//   Posix RingBuffer: 360.29 MB/s
//   Speedup: 1.14x