#pragma once

#include <cstdint>
#include <ctime>

#include "zedbase/time/time_delta.h"

namespace zedbase {

class Time {
 public:
  static Time Now();

  Time() = default;

  static Time FromTimeT(time_t tt);
  time_t ToTimeT() const;

#if defined(IS_LINUX)
  static Time FromTimeSpec(const timespec& ts);
  timespec ToTimeSpec() const;
#endif  // defined(IS_LINUX)

  bool operator==(Time other) const;
  bool operator!=(Time other) const;
  bool operator<(Time other) const;
  bool operator>(Time other) const;
  bool operator<=(Time other) const;
  bool operator>=(Time other) const;

  Time operator+(TimeDelta delta) const;
  Time operator-(TimeDelta delta) const;
  Time& operator+=(TimeDelta delta);
  Time& operator-=(TimeDelta delta);

  TimeDelta operator-(Time other) const;

 private:
  Time(int64_t us_time) : us_time_(us_time) {}

  int64_t us_time_;
};

}  // namespace zedbase