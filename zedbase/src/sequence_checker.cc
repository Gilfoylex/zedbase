#include "zedbase/sequence_checker.h"

#include <utility>

namespace zedbase {

SequenceChecker::SequenceChecker()
    : sequence_id_(detail::CurrentSequenceIdHelper::current_sequence_id_) {}

SequenceChecker::~SequenceChecker() = default;

SequenceChecker::SequenceChecker(SequenceChecker&& other) {
  const bool other_moved_on_valid_sequence = other.CalledOnValidSequence();
  ZED_CHECK(other_moved_on_valid_sequence);

  sequence_id_ = std::exchange(other.sequence_id_, std::nullopt);
}

SequenceChecker& SequenceChecker::operator=(SequenceChecker&& other) {
  ZED_CHECK(CalledOnValidSequence());

  const bool other_moved_on_valid_sequence = other.CalledOnValidSequence();
  ZED_CHECK(other_moved_on_valid_sequence);

  sequence_id_ = std::exchange(other.sequence_id_, std::nullopt);

  return *this;
}

bool SequenceChecker::CalledOnValidSequence() const {
  if (!sequence_id_) {
    sequence_id_ = detail::CurrentSequenceIdHelper::current_sequence_id_;
  }

  ZED_CHECK(sequence_id_) << "Must run while in sequence!";
  return (sequence_id_ ==
          detail::CurrentSequenceIdHelper::current_sequence_id_);
}

void SequenceChecker::DetachFromSequence() {
  if (!sequence_id_) {
    return;
  }

  ZED_CHECK(CalledOnValidSequence());
  sequence_id_.reset();
}

}  // namespace base