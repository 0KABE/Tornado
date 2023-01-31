//
// Created by Chen, WenTao on 2023/1/12.
//

#pragma once

#include <array>
#include <optional>
#include <srs_app_st.hpp>
#include <thread>

namespace Tornado {

enum class NotificationEvent { kPlaceholder };

class TornadoCoroutine : public ISrsCoroutineHandler {
 public:
  constexpr static const char* kCoroutineName = "tornado";

  TornadoCoroutine();

  srs_error_t Start();

  void NotifyOnce(NotificationEvent notification_event) const;

 private:
  srs_error_t cycle() override;

  std::array<int, 2> pipe_{-1, -1};
  srs_netfd_t read_fd_{nullptr};
  std::unique_ptr<SrsCoroutine> coroutine_;
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
