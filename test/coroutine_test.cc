//
// Created by Chen, WenTao on 2023/1/10.
//

#include <gtest/gtest.h>

#include <asio.hpp>
#include <thread>

#include "tornado/base.h"

TEST(Coroutine, Smoke) {  // NOLINT
  using namespace std::chrono_literals;
  constexpr std::chrono::milliseconds kTimeWait = 10ms;

  asio::io_context io_context;
  std::thread::id asio_thread_id;
  auto start = std::chrono::steady_clock::now();

  asio::co_spawn(
      io_context.get_executor(),
      [&] {
        return asio::async_initiate<Tornado::UseAwaitable, void()>(
            [&](auto handler) {
              std::thread([&, handler = std::move(handler)]() mutable {
                std::this_thread::sleep_for(kTimeWait);
                asio_thread_id = std::this_thread::get_id();
                handler();
              }).detach();
            },
            asio::use_awaitable);
      },
      asio::detached);
  io_context.run();

  auto end = std::chrono::steady_clock::now();

  ASSERT_NE(asio_thread_id, std::this_thread::get_id());
  ASSERT_GE(end - start, kTimeWait);
}
