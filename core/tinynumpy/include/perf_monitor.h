#ifndef PERF_MONITOR_H
#define PERF_MONITOR_H

#include <stdint.h>

typedef struct {
  uint64_t value;
  uint64_t time_enabled;
  uint64_t time_running;
} perf_result;

// Initializes the perf event
int setup_perf_event(int type, int config);

// Starts the perf event
void start_perf_event(int fd);

// Stops the perf event
void stop_perf_event(int fd);

// Reads the value of the perf event
int read_perf_event(int fd, perf_result *res);

#endif