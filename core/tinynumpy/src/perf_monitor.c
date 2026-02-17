#define _GNU_SOURCE
#include "perf_monitor.h"
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int setup_perf_event(int type, int config) {
  struct perf_event_attr pe = {.type = type,
                               .size = sizeof(struct perf_event_attr),
                               .config = config,
                               .disabled = 1,
                               .exclude_kernel = 1,
                               .exclude_hv = 1,
                               .read_format = PERF_FORMAT_TOTAL_TIME_ENABLED |
                                              PERF_FORMAT_TOTAL_TIME_RUNNING};

  int fd = syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0);
  if (fd < 0) {
    perror("perf_event_open failed");
    return -1;
  }

  return fd;
}

void start_perf_event(int fd) {
  if (fd < 0) {
    fprintf(stderr, "Error: invalid file descriptor (fd=%d)\n", fd);
    return;
  }
  ioctl(fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}

void stop_perf_event(int fd) {
  if (fd < 0) {
    fprintf(stderr, "Error: invalid file descriptor (fd=%d)\n", fd);
    return;
  }
  ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
}

int read_perf_event(int fd, perf_result *res) {
  if (fd < 0) {
    fprintf(stderr, "Error: invalid file descriptor (fd=%d)\n", fd);
    return -1;
  }

  ssize_t bytes = read(fd, res, sizeof(*res));

  if (bytes != sizeof(*res)) {
    if (bytes == -1)
      perror("read");
    else
      fprintf(stderr, "Error: read %zd bytes\n", bytes);
    return -1;
  }

  return 0;
}