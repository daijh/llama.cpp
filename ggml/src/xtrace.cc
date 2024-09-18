// Author: jianhui.j.dai@intel.com
// A simple tracing tool.
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "xtrace.h"

#ifdef _WIN32

#include <process.h>
#include <thread>
#define x_getpid() _getpid()
// Avoid `GetCurrentThreadId()`
#define x_gettid() std::this_thread::get_id()

#else

#include <sys/syscall.h>
#include <unistd.h>
#define x_getpid() getpid()
#define x_gettid() ((pid_t)syscall(SYS_gettid))

#endif

//= 1000xmilliseconds
static uint64_t xtrace_timestamp_microseconds(void) {
  // Let's not consider overflow or 0s cases.
  static uint64_t start_timestamp = 0;

  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  uint64_t timestamp = UINT64_C(1000000) * ts.tv_sec + ts.tv_nsec / 1000;
  // `start_timestamp` could be set multiple times, as static declared.
#if 0
  if (!start_timestamp) {
    start_timestamp = timestamp;
  }
#endif

  return timestamp - start_timestamp;
}

static void xtrace_write_head(FILE* fp) {
  if (fp) {
    char buf[1024];
    size_t len;

    len = snprintf(buf, 1024, "{\"traceEvents\": [\n");
    fwrite(buf, 1, len, fp);
    fflush(fp);
  }
}

static void xtrace_write_tail(FILE* fp) {
  if (fp) {
    char buf[1024];
    size_t len;

    len = snprintf(buf, 1024,
                   "{\"ts\": %ld, \"cat\": \"metadata\", \"pid\": %d, \"ph\": "
                   "\"%s\", \"name\": \"%s\", \"args\": {\"name\": \"%s\"}}\n",
                   xtrace_timestamp_microseconds(), x_getpid(), "M",
                   "process_name", "xtrace");
    fwrite(buf, 1, len, fp);
    fflush(fp);

    len = snprintf(buf, 1024, "]}\n");
    fwrite(buf, 1, len, fp);
    fflush(fp);
  }
}

static FILE* s_xtrace_fp;
static bool s_xtrace_initialized = false;
static char s_xtrace_output[256];

static void xtrace_deinitialize() {
  if (s_xtrace_fp) {
    xtrace_write_tail(s_xtrace_fp);
    fflush(s_xtrace_fp);
    fclose(s_xtrace_fp);
    s_xtrace_fp = nullptr;

    fprintf(stderr, "Output: %s\n", s_xtrace_output);
  }
}

static void xtrace_initialize(const char* tracing_output) {
  FILE* fp = fopen(tracing_output, "w");
  if (!fp) {
    fprintf(stderr, "Error, can not open: %s\n", tracing_output);
  } else {
    strncpy(s_xtrace_output, tracing_output, 256);
  }
  s_xtrace_fp = fp;
  xtrace_write_head(s_xtrace_fp);

  std::atexit(xtrace_deinitialize);
}

static FILE* xtrace_get_trace_fp(void) {
  if (!s_xtrace_initialized) {
    std::string tracing_output =
        "xtrace_" + std::to_string(x_getpid()) + ".json";
    xtrace_initialize(tracing_output.c_str());
    s_xtrace_initialized = true;
  }

  return s_xtrace_fp;
}

void xtrace_write_begin_event(const char* name) {
  FILE* fp = xtrace_get_trace_fp();
  if (fp) {
    char buf[1024];
    size_t len;

    len = snprintf(buf, 1024,
                   "{\"ts\": %ld, \"cat\": \"trace\", \"pid\": %d, \"tid\": "
                   "%d, \"ph\": \"%s\", \"name\": \"%s\"},\n",
                   xtrace_timestamp_microseconds(), x_getpid(), x_gettid(), "B",
                   name);
    fwrite(buf, 1, len, fp);
    fflush(fp);
  }
}

void xtrace_write_end_event(void) {
  FILE* fp = xtrace_get_trace_fp();
  if (fp) {
    char buf[1024];
    size_t len;

    len =
        snprintf(buf, 1024,
                 "{\"ts\": %ld, \"cat\": \"trace\", \"pid\": %d, \"tid\": %d, "
                 "\"ph\": \"%s\"},\n",
                 xtrace_timestamp_microseconds(), x_getpid(), x_gettid(), "E");
    fwrite(buf, 1, len, fp);
    fflush(fp);
  }
}
