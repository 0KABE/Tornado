//
// Created by Chen, WenTao on 2023/2/5.
//

#pragma once

#include <asio.hpp>
#include <memory>
#include <utility>

#include "srs_instance.h"

namespace Tornado {
class RTMPStream;

class RTMPServer : public std::enable_shared_from_this<RTMPServer> {
 public:
  using RTMPStreamPtr = std::shared_ptr<RTMPStream>;

  static constexpr const char* kRtmpApp = "live";
  static constexpr const char* kRtmpVhost = "okab3";

  RTMPServer(std::string config_path) : config_path_(std::move(config_path)), instance_() {}

  void Run();
  void Stop();
  asio::awaitable<void> AsyncRun();

  std::shared_ptr<RTMPStream> CreateOrGetStream();                           // Sync call
  asio::awaitable<RTMPStreamPtr> AsyncCreateOrGetStream(std::string token);  // Async call

 private:
  std::string config_path_;
  SRSInstance instance_;
};
}  // namespace Tornado
