#pragma once

#include <cstdint>
#include <ctime>

namespace zedbase {

class Time;
class TimeTicks;

class TimeDelta {
 public:
#if defined(IS_LINUX)
  static TimeDelta FromTimeSpec(const timespec& ts);
#endif  // defined(IS_LINUX)

  TimeDelta() = default;

  bool IsZero() const;
  bool IsPositive() const;
  bool IsNegative() const;

  int InDays() const;
  int InHours() const;
  int InMinutes() const;
  double InSecondsF() const;
  int64_t InSeconds() const;
  double InMillisecondsF() const;
  int64_t InMilliseconds() const;
  int64_t InMicroseconds() const;
  double InMicrosecondsF() const;
  int64_t InNanoseconds() const;

  TimeDelta operator+(TimeDelta other) const;
  TimeDelta operator-(TimeDelta other) const;
  TimeDelta& operator+=(TimeDelta other);
  TimeDelta& operator-=(TimeDelta other);
  TimeDelta operator-() const;

  template <typename T>
  TimeDelta operator*(T a) const {
    return TimeDelta{int64_t{us_delta_ * a}};
  }

  template <typename T>
  TimeDelta operator/(T a) const {
    return TimeDelta{int64_t{us_delta_ / a}};
  }

  template <typename T>
  TimeDelta& operator*=(T a) {
    return *this = (*this * a);
  }

  template <typename T>
  TimeDelta& operator/=(T a) {
    return *this = (*this / a);
  }

  bool operator==(TimeDelta other) const;
  bool operator!=(TimeDelta other) const;
  bool operator<(TimeDelta other) const;
  bool operator<=(TimeDelta other) const;
  bool operator>(TimeDelta other) const;
  bool operator>=(TimeDelta other) const;

 private:
  template <typename T>
  friend TimeDelta Days(T);
  template <typename T>
  friend TimeDelta Hours(T);
  template <typename T>
  friend TimeDelta Minutes(T);
  template <typename T>
  friend TimeDelta Seconds(T);
  template <typename T>
  friend TimeDelta Milliseconds(T);
  template <typename T>
  friend TimeDelta Microseconds(T);
  template <typename T>
  friend TimeDelta Nanoseconds(T);

  friend class Time;
  friend class TimeTicks;

  static const int64_t kNanosecondsInMicroseconds = 1000;
  static const int64_t kMicrosecondsInMilliseconds = 1000;
  static const int64_t kMicrosecondsInSeconds =
      1000 * kMicrosecondsInMilliseconds;
  static const int64_t kMicrosecondsInMinutes = 60 * kMicrosecondsInSeconds;
  static const int64_t kMicrosecondsInHours = 60 * kMicrosecondsInMinutes;
  static const int64_t kMicrosecondsInDays = 24 * kMicrosecondsInHours;

  explicit TimeDelta(int64_t us_delta) : us_delta_(us_delta) {}

  int64_t us_delta_;
};

template <typename T>
TimeDelta Days(T n) {
  return TimeDelta{static_cast<int64_t>(n * TimeDelta::kMicrosecondsInDays)};
}

template <typename T>
TimeDelta Hours(T n) {
  return TimeDelta{static_cast<int64_t>(n * TimeDelta::kMicrosecondsInHours)};
}

template <typename T>
TimeDelta Minutes(T n) {
  return TimeDelta{static_cast<int64_t>(n * TimeDelta::kMicrosecondsInMinutes)};
}

template <typename T>
TimeDelta Seconds(T n) {
  return TimeDelta{static_cast<int64_t>(n * TimeDelta::kMicrosecondsInSeconds)};
}

template <typename T>
TimeDelta Milliseconds(T n) {
  return TimeDelta{
      static_cast<int64_t>(n * TimeDelta::kMicrosecondsInMilliseconds)};
}

template <typename T>
TimeDelta Microseconds(T n) {
  return TimeDelta{static_cast<int64_t>(n)};
}

template <typename T>
TimeDelta Nanoseconds(T n) {
  return TimeDelta{
      static_cast<int64_t>(n / TimeDelta::kNanosecondsInMicroseconds)};
}

}  // namespace zedbase