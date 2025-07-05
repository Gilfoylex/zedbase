#include "zedbase/threading/thread.h"

#include <Windows.h>

#include "zedbase/bind.h"
#include "zedbase/callback.h"
#include "zedbase/message_loop/message_loop_impl.h"
#include "zedbase/message_loop/message_pump_impl.h"
#include "zedbase/sequenced_task_runner_helpers.h"
#include "zedbase/synchronization/waitable_event.h"
#include "zedbase/threading/delayed_task_manager_shared_instance.h"
#include "zedbase/threading/task_runner_impl.h"

namespace zedbase {

typedef std::function<void()> ThreadFunction;
class ThreadHandle {
 public:
  explicit ThreadHandle(ThreadFunction&& function);
  ~ThreadHandle();

  void Join();

 private:
  // #if defined(FML_OS_WIN)
  HANDLE thread_;
  // #else
  //   pthread_t thread_;
  // #endif
};

#if defined(IS_WINDOWS)
ThreadHandle::ThreadHandle(ThreadFunction&& function) {
  thread_ = (HANDLE*)_beginthreadex(
      nullptr, static_cast<unsigned int>(Thread::GetDefaultStackSize()),
      [](void* arg) -> unsigned {
        std::unique_ptr<ThreadFunction> function(
            reinterpret_cast<ThreadFunction*>(arg));
        (*function)();
        return 0;
      },
      new ThreadFunction(std::move(function)), 0, nullptr);
  ZED_CHECK(thread_ != nullptr);
}

void ThreadHandle::Join() {
  WaitForSingleObjectEx(thread_, INFINITE, FALSE);
}

ThreadHandle::~ThreadHandle() {
  CloseHandle(thread_);
}
#endif  // defined(IS_WINDOWS)

#if defined(IS_WINDOWS)

const DWORD kVCThreadNameException = 0x406D1388;
typedef struct tagTHREADNAME_INFO {
  DWORD dwType;      // Must be 0x1000.
  LPCSTR szName;     // Pointer to name (in user addr space).
  DWORD dwThreadID;  // Thread ID (-1=caller thread).
  DWORD dwFlags;     // Reserved for future use, must be zero.
} THREADNAME_INFO;
// #endif

void SetThreadName(const std::string& name) {
  if (name == "") {
    return;
  }
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = name.c_str();
  info.dwThreadID = GetCurrentThreadId();
  info.dwFlags = 0;
  __try {
    RaiseException(kVCThreadNameException, 0, sizeof(info) / sizeof(DWORD),
                   reinterpret_cast<DWORD_PTR*>(&info));
  } __except (EXCEPTION_CONTINUE_EXECUTION) {
  }
}

void Thread::SetCurrentThreadName(const std::string& name) {
  SetThreadName(name);
}

#endif  // defined(IS_WINDOWS)

Thread::Thread() : Thread("") {}

Thread::Thread(std::string name) {
  const size_t executor_count = 1;
  auto message_pump = std::make_shared<MessagePumpImpl>(executor_count);

  const MessagePump::ExecutorId executor_id = 0;
  message_loop_ = std::make_unique<MessageLoopImpl>(executor_id, message_pump);
  //   thread_ =
  //       std::make_unique<std::thread>(&MessageLoop::Run,
  //       message_loop_.get());
  auto thread_func = std::bind(&MessageLoop::Run, message_loop_.get());
  thread_ = std::make_unique<ThreadHandle>(
      [name, thread_func = std::move(thread_func)]() -> void {
        Thread::SetCurrentThreadName(name);
        thread_func();
      });
  std::weak_ptr<MessagePump> weak_message_pump = message_pump;
  sequence_id_ = detail::SequenceIdGenerator::GetNextSequenceId();
  task_runner_ = SingleThreadTaskRunnerImpl::Create(
      weak_message_pump, *sequence_id_, executor_id,
      DelayedTaskManagerSharedInstance::GetOrCreateSharedInstance());
}

Thread::~Thread() {
  Stop();
}

// std::thread::id Thread::Id() const {
//   if (!thread_) {
//     return {};
//   }
//   return thread_->get_id();
// }

void Thread::Stop() {
  Stop(FROM_HERE, zedbase::OnceClosure{});
}

void Thread::Stop(SourceLocation location, OnceClosure last_task) {
  if (message_loop_) {
    ZED_DCHECK(sequence_id_);
    (void)location;  // TODO: use `location`
    message_loop_->Stop(
        MessagePump::PendingTask{std::move(last_task), *sequence_id_,
                                 MessagePump::ExecutorId{0}, task_runner_});
  }
  if (thread_) {
    thread_->Join();
  }

  thread_.reset();
  message_loop_.reset();
  sequence_id_.reset();
  task_runner_.reset();
}

std::shared_ptr<SingleThreadTaskRunner> Thread::TaskRunner() {
  return task_runner_;
}

void Thread::FlushForTesting() {
  if (!thread_) {
    return;
  }

  AutoResetWaitableEvent waitable_event;
  TaskRunner()->PostTask(
      FROM_HERE,
      BindOnce([](AutoResetWaitableEvent* event) { event->Signal(); },
               &waitable_event));
  waitable_event.Wait();
}

size_t Thread::GetDefaultStackSize() {
  return 1024 * 1024 * 2;
}

}  // namespace zedbase