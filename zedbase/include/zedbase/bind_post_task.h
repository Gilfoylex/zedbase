#pragma once

#include <type_traits>

#include "zedbase/callback.h"
#include "zedbase/task_runner.h"
#include "zedbase/threading/sequenced_task_runner_handle.h"

namespace zedbase {

namespace detail {
template <typename Callback, typename... Arguments>
class BindPostTaskHelper {
 public:
  BindPostTaskHelper(std::shared_ptr<TaskRunner> task_runner,
                     Callback callback,
                     SourceLocation location)
      : task_runner_(std::move(task_runner)),
        callback_(std::move(callback)),
        location_(std::move(location)) {}

  void Run(Arguments... arguments) {
    if constexpr (traits::IsOnceCallbackV<Callback>) {
      task_runner_->PostTask(location_,
                             BindOnce(std::move(callback_),
                                      std::forward<Arguments>(arguments)...));
    } else {
      task_runner_->PostTask(
          location_,
          BindOnce(callback_, std::forward<Arguments>(arguments)...));
    }
  }

 private:
  std::shared_ptr<TaskRunner> task_runner_;
  Callback callback_;
  SourceLocation location_;
};
}  // namespace detail

template <typename Return, typename... Arguments>
OnceCallback<Return(Arguments...)> BindPostTask(
    std::shared_ptr<TaskRunner> task_runner,
    OnceCallback<Return(Arguments...)> callback,
    SourceLocation location) {
  static_assert(std::is_same_v<Return, void>,
                "Cannot BindPostTask callback with non-void return type");

  using Helper = detail::BindPostTaskHelper<OnceCallback<Return(Arguments...)>,
                                            Arguments...>;
  return BindOnce(&Helper::Run, zedbase::Owned(std::make_unique<Helper>(
                                    std::move(task_runner), std::move(callback),
                                    std::move(location))));
}

template <typename Return, typename... Arguments>
RepeatingCallback<Return(Arguments...)> BindPostTask(
    std::shared_ptr<TaskRunner> task_runner,
    RepeatingCallback<Return(Arguments...)> callback,
    SourceLocation location) {
  static_assert(std::is_same_v<Return, void>,
                "Cannot BindPostTask callback with non-void return type");

  using Helper =
      detail::BindPostTaskHelper<RepeatingCallback<Return(Arguments...)>,
                                 Arguments...>;
  return BindRepeating(
      &Helper::Run,
      zedbase::Owned(std::make_unique<Helper>(
          std::move(task_runner), std::move(callback), std::move(location))));
}

template <typename Return, typename... Arguments>
OnceCallback<Return(Arguments...)> BindToCurrentSequence(
    OnceCallback<Return(Arguments...)> callback,
    SourceLocation location) {
  static_assert(
      std::is_same_v<Return, void>,
      "Cannot BinToCurrentSequence callback with non-void return type");

  return BindPostTask(SequencedTaskRunnerHandle::Get(), std::move(callback),
                      std::move(location));
}

template <typename Return, typename... Arguments>
RepeatingCallback<Return(Arguments...)> BindToCurrentSequence(
    RepeatingCallback<Return(Arguments...)> callback,
    SourceLocation location) {
  static_assert(
      std::is_same_v<Return, void>,
      "Cannot BinToCurrentSequence callback with non-void return type");

  return BindPostTask(SequencedTaskRunnerHandle::Get(), std::move(callback),
                      std::move(location));
}

}  // namespace zedbase