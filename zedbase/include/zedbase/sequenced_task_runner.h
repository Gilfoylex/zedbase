#pragma once

#include "zedbase/bind.h"
#include "zedbase/sequenced_task_runner_internals.h"
#include "zedbase/task_runner.h"

namespace zedbase {

class SequencedTaskRunner : public TaskRunner {
 public:
  // Returns true if:
  // - this is a SequencedTaskRunner to which the current task was posted,
  // - this is a SequencedTaskRunner bound to the same sequence as the
  //   SequencedTaskRunner to which the current task was posted,
  // - this is a SingleThreadTaskRunner bound to the current thread.
  virtual bool RunsTasksInCurrentSequence() const = 0;

  template <typename T>
  bool DeleteSoon(SourceLocation location, std::unique_ptr<T> object) {
    return PostTask(
        std::move(location),
        BindOnce(&detail::DeleteSoonUniquePtr<T>, object.release()));
  }

  template <typename T>
  bool DeleteSoon(SourceLocation location, const T* object) {
    return PostTask(std::move(location),
                    BindOnce(&detail::DeleteSoonPtr<T>, object));
  }
};

}  // namespace zedbase