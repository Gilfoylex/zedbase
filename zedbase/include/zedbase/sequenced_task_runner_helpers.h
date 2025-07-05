#pragma once

#include <atomic>
#include <optional>

#include "zedbase/sequence_id.h"

namespace zedbase {

class SequenceChecker;

namespace detail {

class SequenceIdGenerator {
 public:
  static SequenceId GetNextSequenceId();
};

class CurrentSequenceIdHelper {
 public:
  static bool IsCurrentSequence(const SequenceId& sequence_id);

 private:
  friend class ScopedSequenceIdSetter;
  friend class ::zedbase::SequenceChecker;

  inline static thread_local std::optional<SequenceId> current_sequence_id_ =
      std::nullopt;
};

class ScopedSequenceIdSetter {
 public:
  explicit ScopedSequenceIdSetter(SequenceId current_sequence_id);
  ~ScopedSequenceIdSetter();
};

}  // namespace detail
}  // namespace zedbase