#pragma once

#include "zedbase/callback_forward.h"
#include "zedbase/message_loop/message_pump.h"

namespace zedbase {

class MessageLoop {
 public:
  virtual ~MessageLoop() = default;

  virtual bool RunOnce() = 0;
  virtual void RunUntilIdle() = 0;

  virtual void Run() = 0;
  virtual void Stop(MessagePump::PendingTask last_task) = 0;
};

}  // namespace base