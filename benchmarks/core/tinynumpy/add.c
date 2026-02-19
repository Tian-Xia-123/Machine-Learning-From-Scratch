#define _POSIX_C_SOURCE 199309L
#include "perf_monitor.h"
#include "tinynumpy.h"
#include <inttypes.h>
#include <linux/perf_event.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main() {
  int N = 50000000;
  int shape[] = {N};
  int ndim = 1;

  int fd_access = setup_perf_event(PERF_TYPE_HW_CACHE,
                                   PERF_COUNT_HW_CACHE_L1D |
                                       PERF_COUNT_HW_CACHE_OP_READ << 8 |
                                       PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16);
  int fd_miss = setup_perf_event(PERF_TYPE_HW_CACHE,
                                 PERF_COUNT_HW_CACHE_L1D |
                                     PERF_COUNT_HW_CACHE_OP_READ << 8 |
                                     PERF_COUNT_HW_CACHE_RESULT_MISS << 16);

  ndarray *a = np_array(ndim, shape);
  ndarray *b = np_array(ndim, shape);
  if (a == NULL || b == NULL) {
    return -1;
  }

  for (int i = 0; i < N; i++) {
    a->data[i] = 1.0;
    b->data[i] = 2.0;
  }

  // Warm up the CPU cache and trigger lazy memory allocation
  ndarray *dummy = np_add(a, b);
  np_free(dummy);

  start_perf_event(fd_access);
  start_perf_event(fd_miss);

  // Core benchmark logic
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  ndarray *res = np_add(a, b);
  clock_gettime(CLOCK_MONOTONIC, &end);

  stop_perf_event(fd_access);
  stop_perf_event(fd_miss);

  perf_result *access_res = malloc(sizeof(*access_res));
  perf_result *miss_res = malloc(sizeof(*miss_res));

  if (access_res == NULL || miss_res == NULL) {
    perror("malloc failed");
    return -1;
  }

  if (read_perf_event(fd_access, access_res) == -1 ||
      read_perf_event(fd_miss, miss_res) == -1) {
    return -1;
  }

  printf("Array Size: %d\n", N);
  printf("Cache L1D Accesses: %" PRIu64 ", Misses: %" PRIu64
         ", Miss Rate: %.2f%%\n",
         access_res->value, miss_res->value,
         (double)miss_res->value / access_res->value * 100.0);
  double time_taken =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  printf("Absolute latency: %f seconds\n", time_taken);
  printf("Computational Throughput: %f MB/s\n",
         (N * sizeof(double) * 3) / (time_taken * 1024 * 1024));

  np_free(a);
  np_free(b);
  np_free(res);
  close(fd_access);
  close(fd_miss);

  return 0;
}