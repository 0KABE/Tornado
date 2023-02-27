//
// Created by Chen, WenTao on 2023/2/6.
//

#pragma once

#include <asio.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <srs_app_source.hpp>
#include <utility>

#include "event.h"
#include "srs_kernel_flv.hpp"

namespace Tornado {

struct Message {
  using PayloadItem = uint8_t;

  size_t size;
  std::unique_ptr<PayloadItem[]> payload;

  Message() = default;

  explicit Message(size_t size) : size(size), payload(std::make_unique<PayloadItem[]>(size)) {}

  Message(size_t size, PayloadItem* src_payload) : Message(size) {
    memcpy(payload.get(), src_payload, sizeof(PayloadItem) * size);
  }
};

using VideoMessage = Message;

class RTMPStream : public std::enable_shared_from_this<RTMPStream> {
 public:
  using SourceDeleter = void (*)(SrsLiveSource*);

  explicit RTMPStream(SrsLiveSource* srs_live_source);

  ~RTMPStream() {}

  void RegisterBridge();

  asio::awaitable<VideoMessage> AsyncFetchVideoMessage();

  asio::awaitable<VideoMessage> AsyncFetchVideoMessageInternal();

 private:
  static void ReleaseSource(SrsLiveSource* srs_live_source);

  struct SrsLiveSourceBridger : public ISrsLiveSourceBridger {
    explicit SrsLiveSourceBridger(std::shared_ptr<RTMPStream> rtmp_stream)
        : rtmp_stream_(std::move(rtmp_stream)) {}

    ~SrsLiveSourceBridger() = default;

    srs_error_t on_publish() override;
    srs_error_t on_audio(SrsSharedPtrMessage* audio) override;
    srs_error_t on_video(SrsSharedPtrMessage* video) override;
    void on_unpublish() override;

    std::shared_ptr<RTMPStream> rtmp_stream_;
  };

  std::mutex mutex_;
  std::optional<SrsSharedPtrMessage> video_message_;
  std::optional<SrsSharedPtrMessage> audio_message_;
  EventPtr video_waiter_ = nullptr;
  EventPtr audio_waiter_ = nullptr;
  SrsLiveSourceBridger* live_source_bridger_ = nullptr;

  std::unique_ptr<SrsLiveSource, SourceDeleter> srs_live_source_;
};
}  // namespace Tornado
