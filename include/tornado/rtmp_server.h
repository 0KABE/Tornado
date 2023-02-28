//
// Created by Chen, WenTao on 2023/2/5.
//

#pragma once

#include <spdlog/spdlog.h>

#include <asio.hpp>
#include <memory>
#include <thread>
#include <utility>

#include "srs_instance.h"

namespace Tornado {
class RTMPStream;

class RTMPServer : public std::enable_shared_from_this<RTMPServer> {
 public:
  using RTMPStreamPtr = std::shared_ptr<RTMPStream>;

  static constexpr const char* kRtmpApp = "live";
  static constexpr const char* kRtmpVhost = "/okab3";

  explicit RTMPServer(std::string config_path);

  ~RTMPServer() {
    spdlog::info("~RTMPServer();");
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  void Run();
  void Stop();

  //  RTMPStreamPtr CreateOrGetStream();                                         // Sync call
  asio::awaitable<RTMPStreamPtr> AsyncCreateOrGetStream(std::string token);  // Async call

 private:
  std::thread thread_;
  std::string config_path_;
  SRSInstance instance_;
};
}  // namespace Tornado
