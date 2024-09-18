#ifndef XTRACE_H
#define XTRACE_H

#define __XTRACE_FUNCTION__ __func__

void xtrace_write_begin_event(const char* name);
void xtrace_write_end_event(void);

#define XTRACE_BEGIN(name) xtrace_write_begin_event(name)
#define XTRACE_END() xtrace_write_end_event()

#ifdef __cplusplus

#include <string>

namespace xtrace {
class ScopedTrace {
 public:
  inline ScopedTrace(const char* name) { XTRACE_BEGIN(name); }

  inline ~ScopedTrace() { XTRACE_END(); }
};
}  // namespace xtrace

#define XTRACE_CAT(a, b) XTRACE_CAT_I(a, b)
#define XTRACE_CAT_I(a, b) XTRACE_CAT_II(~, a##b)
#define XTRACE_CAT_II(p, res) res

#define XTRACER XTRACE_CAT(xtracer, __LINE__)

// XTRACE_NAME traces the beginning and end of the current scope.  To trace
// the correct start and end times this macro should be declared first in the
// scope body.
#define XTRACE_NAME(name) xtrace::ScopedTrace XTRACER(name)

// XTRACE_CALL is an XTRACE_NAME that uses the current function name.
#define XTRACE_CALL() XTRACE_NAME(__XTRACE_FUNCTION__)

#endif  //__cplusplus

#endif  // XTRACE_H
