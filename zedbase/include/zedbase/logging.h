#pragma once

#include <sstream>
#include "zedbase/log_level.h"
#include "zedbase/macros.h"

namespace zedbase {

namespace testing {
struct LogCapture {
  LogCapture();
  ~LogCapture();

  std::string str() const;

 private:
  std::ostringstream stream_;
};
}  // namespace testing

class LogMessageVoidify {
 public:
  void operator&(std::ostream&) {}
};

class LogMessage {
 public:
  LogMessage(LogSeverity severity,
             const char* file,
             int line,
             const char* condition);
  ~LogMessage();

  std::ostream& stream() { return stream_; }

  static void CaptureNextLog(std::ostringstream* stream);

 private:
  // This is a raw pointer so that we avoid having a non-trivially-destructible
  // static. It is only ever for use in unit tests.
  static thread_local std::ostringstream* capture_next_log_stream_;
  std::ostringstream stream_;
  const LogSeverity severity_;
  const char* file_;
  const int line_;

  ZED_DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

// Gets the FML_VLOG default verbosity level.
int GetVlogVerbosity();

// Returns true if |severity| is at or above the current minimum log level.
// kLogFatal and above is always true.
bool ShouldCreateLogMessage(LogSeverity severity);

constexpr bool ShouldCreateLogMessageConstexpr(LogSeverity severity,
                                               bool true_arg) {
  if (true_arg) {
    return ShouldCreateLogMessage(severity);
  }
  return false;
}

[[noreturn]] void KillProcess();

[[noreturn]] constexpr void KillProcessConstexpr(bool true_arg) {
  if (true_arg) {
    KillProcess();
  }
#if defined(_MSC_VER) && !defined(__clang__)
  __assume(false);
#else   // defined(_MSC_VER) && !defined(__clang__)
  __builtin_unreachable();
#endif  // defined(_MSC_VER) && !defined(__clang__)
}

}  // namespace zedbase

#define ZED_LOG_STREAM(severity)                                            \
  ::zedbase::LogMessage(::zedbase::##severity, __FILE__, __LINE__, nullptr) \
      .stream()

#define ZED_LAZY_STREAM(stream, condition) \
  !(condition) ? (void)0 : ::zedbase::LogMessageVoidify() & (stream)

#define ZED_EAT_STREAM_PARAMETERS(ignored) \
  true || (ignored)                        \
      ? (void)0                            \
      : ::zedbase::LogMessageVoidify() &   \
            ::zedbase::LogMessage(::fml::kLogFatal, 0, 0, nullptr).stream()

#define ZED_LOG_IS_ON(severity) \
  (::zedbase::ShouldCreateLogMessageConstexpr(::zedbase::##severity, true))

#define ZED_LOG(severity) \
  ZED_LAZY_STREAM(ZED_LOG_STREAM(severity), ZED_LOG_IS_ON(severity))

#define ZED_CHECK(condition)                                            \
  ZED_LAZY_STREAM(::zedbase::LogMessage(::zedbase::kLogFatal, __FILE__, \
                                        __LINE__, #condition)           \
                      .stream(),                                        \
                  !(condition))

#define ZED_VLOG_IS_ON(verbose_level) \
  ((verbose_level) <= ::zedbase::GetVlogVerbosity())

// The VLOG macros log with negative verbosities.
#define ZED_VLOG_STREAM(verbose_level) \
  ::zedbase::LogMessage(-verbose_level, __FILE__, __LINE__, nullptr).stream()

#define ZED_VLOG(verbose_level) \
  ZED_LAZY_STREAM(ZED_VLOG_STREAM(verbose_level), ZED_VLOG_IS_ON(verbose_level))

#ifndef NDEBUG
#define ZED_DLOG(severity) ZED_LOG(severity)
#define ZED_DCHECK(condition) ZED_CHECK(condition)
#else
#define ZED_DLOG(severity) ZED_EAT_STREAM_PARAMETERS(true)
#define ZED_DCHECK(condition) ZED_EAT_STREAM_PARAMETERS(condition)
#endif

#define ZED_UNREACHABLE()                              \
  {                                                    \
    ZED_LOG(kLogError) << "Reached unreachable code."; \
    ::zedbase::KillProcessConstexpr(true);             \
  }
