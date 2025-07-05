#pragma once

#include <memory>
#include <optional>
#include <string>
#include <thread>


#include "zedbase/callback_forward.h"
#include "zedbase/message_loop/message_loop.h"
#include "zedbase/sequence_id.h"
#include "zedbase/single_thread_task_runner.h"
#include "zedbase/source_location.h"

namespace zedbase {

class ThreadHandle;

class Thread {
 public:
  Thread();
  explicit Thread(std::string name);
  ~Thread();

  void Stop();
  void Stop(SourceLocation location, OnceClosure last_task);

  // std::thread::id Id() const;
  std::shared_ptr<SingleThreadTaskRunner> TaskRunner();

  void FlushForTesting();
  
  static void SetCurrentThreadName(const std::string& name);

  static size_t GetDefaultStackSize();

 private:
  std::unique_ptr<MessageLoop> message_loop_;
  std::unique_ptr<ThreadHandle> thread_;
  std::optional<zedbase::SequenceId> sequence_id_;
  std::shared_ptr<SingleThreadTaskRunner> task_runner_;
};

}  // namespace zedbase