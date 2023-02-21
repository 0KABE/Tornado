//
// Created by Chen, WenTao on 2023/2/5.
//

#include "rtmp_server.h"

#include <fmt/format.h>

#include <srs_app_hybrid.hpp>
#include <srs_app_server.hpp>
#include <srs_app_source.hpp>
#include <srs_protocol_amf0.hpp>
#include <srs_rtmp_stack.hpp>

#include "base.h"
#include "rtmp_stream.h"

Tornado::RTMPServer::RTMPServer(std::string config_path)
    : config_path_(std::move(config_path)), instance_() {}

void Tornado::RTMPServer::Run() {
  instance_.Run(fmt::format("srs -c {}", config_path_));
  spdlog::info("RTMPServer::Run() completed");
}

void Tornado::RTMPServer::Stop() { instance_.FastStop(); }

asio::awaitable<Tornado::RTMPServer::RTMPStreamPtr> Tornado::RTMPServer::AsyncCreateOrGetStream(
    std::string token) {
  return asio::async_initiate<decltype(asio::use_awaitable), void(RTMPStreamPtr)>(
      [self = shared_from_this(), token](auto handler) {
        // std::function is not copyable if the captured parameters is not copyable,
        // use std::shared_ptr instead
        // submit task to SRS thread to execute
        self->instance_.GetControlCoroutine().PushTask(
            [token,
             handler = std::make_shared<decltype(handler)>(std::move(handler))]() mutable noexcept {
              SrsRequest srs_request;
              srs_request.args = SrsAmf0Any::object();
              srs_request.vhost = kRtmpVhost;
              srs_request.app = kRtmpApp;
              srs_request.stream = token;

              SrsLiveSource* srs_live_source = nullptr;
              srs_error_t err = _srs_sources->fetch_or_create(
                  &srs_request, _srs_hybrid->srs()->instance(), &srs_live_source);

              if (err != srs_success) {
                srs_error("Failed, %s", srs_error_desc(err).c_str());
              }
              srs_freep(err);

              (*handler)(std::make_shared<RTMPStream>(srs_live_source));
            });
        self->instance_.GetControlCoroutine().NotifyOnce(NotificationEvent::kTask);
      },
      asio::use_awaitable);
}
