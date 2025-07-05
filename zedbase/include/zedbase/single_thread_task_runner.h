#pragma once

#include "zedbase/sequenced_task_runner.h"

namespace zedbase {

class SingleThreadTaskRunner : public SequencedTaskRunner {
 public:
  // Alias for `RunsTasksInCurrentSequence()`.
  bool BelongsToCurrentThread() const { return RunsTasksInCurrentSequence(); }
};

}  // namespace zedbase