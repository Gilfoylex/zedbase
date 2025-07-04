#pragma once

namespace zedbase {
// Default log levels. Negative values can be used for verbose log levels.
typedef int LogSeverity;

constexpr LogSeverity kLogInfo = 0;
constexpr LogSeverity kLogWarning = 1;
constexpr LogSeverity kLogError = 2;
// A log that is not an error, is important enough to display even if ordinary
// info is hidden.
constexpr LogSeverity kLogImportant = 3;
constexpr LogSeverity kLogFatal = 4;
constexpr LogSeverity kLogNumSeverities = 5;

// One of the Windows headers defines ERROR to 0. This makes the token
// concatenation in ZED_LOG(ERROR) to resolve to LOG_0. We define this back to
// the appropriate log level.
#ifdef _WIN32
#define LOG_0 kLogError
#endif

// kLogDFatal is kLogFatal in debug mode, kLogError in normal mode
#ifdef NDEBUG
const LogSeverity kLogDFatal = kLogError;
#else
const LogSeverity kLogDFatal = kLogFatal;
#endif
}  // namespace zedbase