//
// Created by Chen, WenTao on 2023/1/12.
//

#pragma once

#include <array>
#include <functional>
#include <optional>
#include <queue>
#include <srs_app_st.hpp>
#include <thread>

namespace Tornado {

enum class NotificationEvent { kPlaceholder, kTask };

class TornadoCoroutine : public ISrsCoroutineHandler {
 public:
  using Task = std::function<void()>;

  constexpr static const char* kCoroutineName = "tornado";

  TornadoCoroutine();

  srs_error_t Start();

  void PushTask(Task task) const;

  void NotifyOnce(NotificationEvent notification_event) const;

 private:
  srs_error_t cycle() override;

  std::array<int, 2> pipe_{-1, -1};
  srs_netfd_t read_fd_{nullptr};
  std::unique_ptr<SrsCoroutine> coroutine_;

  mutable std::mutex mutex_;
  mutable std::queue<Task> tasks_;
};

class SRSInstance {
 public:
  SRSInstance() = default;

  /**
   *
   * @param arguments
   * @note sync version, guarantee that the `TornadoCoroutine` initialization is complete
   */
  void Run(const std::string& arguments);

  /**
   * Like as gracefully quit in SRS, thread will not stop until there are no connection
   */
  void GracefullyStop();

  /**
   * Like as fast quit in SRS, thread will stop quickly.
   *
   * @note The behavior can be modified to force gracefully quit in config
   */
  void FastStop();

  [[nodiscard]] const TornadoCoroutine& GetControlCoroutine() const;

  ~SRSInstance() = default;

 private:
  std::unique_ptr<TornadoCoroutine> tornado_coroutine_;
  std::thread srs_thread_;
};

}  // namespace Tornado
