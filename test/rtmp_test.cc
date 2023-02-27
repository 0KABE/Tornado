//
// Created by Chen, WenTao on 2023/2/10.
//

#include <gtest/gtest.h>

#include <asio.hpp>

#include "tornado/rtmp_server.h"
#include "tornado/rtmp_stream.h"

using namespace Tornado;

constexpr static const char* kConfigEnv = "SRS_Config";

TEST(RTMPServer, Startup) {
  const char* config_path = getenv(kConfigEnv);
  ASSERT_NE(config_path, nullptr);

  std::shared_ptr<RTMPServer> rtmp_server = std::make_shared<RTMPServer>(config_path);

  asio::io_context io_context;

  rtmp_server->Run();

  asio::co_spawn(
      io_context.get_executor(),
      [rtmp_server]() -> asio::awaitable<void> {
        auto executor = co_await asio::this_coro::executor;
        asio::steady_timer timer(executor);

        timer.expires_after(std::chrono::seconds(10));
        co_await timer.async_wait(asio::use_awaitable);
        rtmp_server->Stop();
      },
      asio::detached);

  asio::co_spawn(
      io_context.get_executor(),
      [rtmp_server]() -> asio::awaitable<void> {
        auto executor = co_await asio::this_coro::executor;
        asio::steady_timer timer(executor);

        timer.expires_after(std::chrono::seconds(3));
        co_await timer.async_wait(asio::use_awaitable);

        constexpr const char* kToken = "test";
        spdlog::info("AsyncCreateOrGetStream: {}", kToken);
        std::shared_ptr<RTMPStream> stream = co_await rtmp_server->AsyncCreateOrGetStream(kToken);

        constexpr unsigned kCount = 20;
        for (unsigned i = 0; i < kCount; ++i) {
          auto message = co_await stream->AsyncFetchVideoMessage();
          spdlog::info("received video messages");
        }
        spdlog::info("received {} video messages", kCount);
      },
      asio::detached);

  io_context.run();
}