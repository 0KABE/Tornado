//
// Created by Chen, WenTao on 2023/2/10.
//

#include <gtest/gtest.h>

#include <asio.hpp>

#include "tornado/rtmp_server.h"

using namespace Tornado;

constexpr static const char* kConfigEnv = "SRS_Config";

TEST(RTMPServer, Startup) {
  const char* config_path = getenv(kConfigEnv);
  ASSERT_NE(config_path, nullptr);

  std::shared_ptr<RTMPServer> rtmp_server = std::make_shared<RTMPServer>(config_path);

  asio::io_context io_context;

  //  asio::co_spawn(
  //      io_context.get_executor(),
  //      [rtmp_server]() -> asio::awaitable<void> {
  //        co_await rtmp_server->AsyncRun();
  //        spdlog::info("Call rtmp_server AsyncRun() completed");
  //      },
  //      asio::detached);
  rtmp_server->Run();

  asio::co_spawn(
      io_context.get_executor(),
      [rtmp_server]() -> asio::awaitable<void> {
        auto executor = co_await asio::this_coro::executor;
        asio::steady_timer timer(executor);

        timer.expires_after(std::chrono::milliseconds(1000));
        co_await timer.async_wait(asio::use_awaitable);
        auto stream = co_await rtmp_server->AsyncCreateOrGetStream("test");

        timer.expires_after(std::chrono::milliseconds(500));
        co_await timer.async_wait(asio::use_awaitable);
        rtmp_server->Stop();
      },
      asio::detached);

  io_context.run();
}