#include "zedbase/threading/sequenced_task_runner_handle.h"

#include "zedbase/logging.h"
#include "zedbase/sequenced_task_runner.h"

namespace zedbase {

namespace {
thread_local SequencedTaskRunnerHandle* g_handle = nullptr;
}

// static
const std::shared_ptr<SequencedTaskRunner>& SequencedTaskRunnerHandle::Get() {
  ZED_CHECK(g_handle) << "Attempted to access SequencedTaskRunner handle on a "
                     "thread without task runner";
  return g_handle->task_runner_;
}

// static
bool SequencedTaskRunnerHandle::IsSet() {
  return g_handle;
}

SequencedTaskRunnerHandle::SequencedTaskRunnerHandle(
    std::shared_ptr<SequencedTaskRunner> task_runner)
    : task_runner_(std::move(task_runner)) {
  ZED_DCHECK(task_runner_);
  ZED_DCHECK(task_runner_->RunsTasksInCurrentSequence());
  ZED_CHECK(!IsSet());

  g_handle = this;
}

SequencedTaskRunnerHandle::~SequencedTaskRunnerHandle() {
  ZED_DCHECK(task_runner_->RunsTasksInCurrentSequence());
  //CHECK_EQ(g_handle, this);
  ZED_CHECK((g_handle == this));

  g_handle = nullptr;
}

}  // namespace base