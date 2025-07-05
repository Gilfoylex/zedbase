#pragma once

#include <optional>

#include "zedbase/logging.h"
#include "zedbase/sequenced_task_runner_helpers.h"

namespace zedbase {

#ifndef NDEBUG

#define SEQUENCE_CHECKER(name) ::zedbase::SequenceChecker name
#define DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker) \
  ZED_DCHECK((sequence_checker).CalledOnValidSequence());
#define DETACH_FROM_SEQUENCE(sequence_checker) \
  (sequence_checker).DetachFromSequence()

#else

#define SEQUENCE_CHECKER(name) static_assert(true, "")
#define DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker)
#define DETACH_FROM_SEQUENCE(sequence_checker)

#endif

class SequenceChecker {
 public:
  SequenceChecker();
  ~SequenceChecker();

  SequenceChecker(SequenceChecker&& other);
  SequenceChecker& operator=(SequenceChecker&& other);

  bool CalledOnValidSequence() const;
  void DetachFromSequence();

 private:
  mutable std::optional<SequenceId> sequence_id_;
};

}  // namespace zedbase