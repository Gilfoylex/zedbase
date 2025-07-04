#include <algorithm>
#include <cstring>
#include <iostream>

#include "zedbase/log_settings.h"
#include "zedbase/logging.h"

namespace zedbase {
namespace {
const char* const kLogSeverityNames[kLogNumSeverities] = {
    "INFO", "WARNING", "ERROR", "IMPORTANT", "FATAL"};

const char* GetNameForLogSeverity(LogSeverity severity) {
  if (severity >= kLogInfo && severity < kLogNumSeverities) {
    return kLogSeverityNames[severity];
  }
  return "UNKNOWN";
}

const char* StripDots(const char* path) {
  while (strncmp(path, "../", 3) == 0) {
    path += 3;
  }
  return path;
}

}  // namespace

LogMessage::LogMessage(LogSeverity severity,
                       const char* file,
                       int line,
                       const char* condition)
    : severity_(severity), file_(StripDots(file)), line_(line) {
#if !defined(OS_FUCHSIA)
  stream_ << "[";
  if (severity >= kLogInfo) {
    stream_ << GetNameForLogSeverity(severity);
  } else {
    stream_ << "VERBOSE" << -severity;
  }
  stream_ << ":" << file_ << "(" << line_ << ")] ";
#endif

  if (condition) {
    stream_ << "Check failed: " << condition << ". ";
  }
}

// static
thread_local std::ostringstream* LogMessage::capture_next_log_stream_ = nullptr;

namespace testing {

LogCapture::LogCapture() {
  zedbase::LogMessage::CaptureNextLog(&stream_);
}

LogCapture::~LogCapture() {
  zedbase::LogMessage::CaptureNextLog(nullptr);
}

std::string LogCapture::str() const {
  return stream_.str();
}

}  // namespace testing

// static
void LogMessage::CaptureNextLog(std::ostringstream* stream) {
  LogMessage::capture_next_log_stream_ = stream;
}

LogMessage::~LogMessage() {
  stream_ << std::endl;
  if (capture_next_log_stream_) {
    *capture_next_log_stream_ << stream_.str();
    capture_next_log_stream_ = nullptr;
  } else {
    // Don't use std::cerr here, because it may not be initialized properly yet.
    fprintf(stderr, "%s", stream_.str().c_str());
    fflush(stderr);
  }

  if (severity_ >= kLogFatal) {
    KillProcess();
  }
}

int GetVlogVerbosity() {
  return std::max(-1, kLogInfo - GetMinLogLevel());
}

bool ShouldCreateLogMessage(LogSeverity severity) {
  return severity >= GetMinLogLevel();
}

void KillProcess() {
  abort();
}

}  // namespace zedbase