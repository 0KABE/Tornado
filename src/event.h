//
// Created by Chen, WenTao on 2023/2/27.
//

#pragma once

#include <spdlog/spdlog.h>

#include <asio.hpp>

namespace Tornado {

class Event : public std::enable_shared_from_this<Event> {
 public:
  explicit Event(const asio::any_io_executor& executor)
      : executor_(executor), timer_(executor, std::chrono::steady_clock::time_point::max()) {}

  asio::awaitable<void> AsyncWait() {
    spdlog::trace("AsyncWait");
    try {
      co_await timer_.async_wait(asio::use_awaitable);
    } catch (asio::system_error& e) {
      if (e.code() != asio::error::operation_aborted) {
        spdlog::error("timer async_wait error: {}[{}]", e.code().message(), e.code().value());
      }
    }
  }

  void NotifyOnce() {
    spdlog::trace("NotifyOnce");
    asio::post(executor_, [self = shared_from_this()]() { self->timer_.cancel_one(); });
  }

  void NotifyAll() {
    spdlog::trace("NotifyAll");
    asio::post(executor_, [self = shared_from_this()]() { self->timer_.cancel(); });
  }

  void ContinuouslyNotify() {
    spdlog::trace("ContinuouslyNotify");
    asio::post(executor_, [self = shared_from_this()]() {
      self->timer_.expires_at(std::chrono::steady_clock::now());
    });
  }

 private:
  const asio::any_io_executor& executor_;
  asio::steady_timer timer_;
};

using EventPtr = std::shared_ptr<Event>;

}  // namespace Tornado
