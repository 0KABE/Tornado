//
// Created by Chen, WenTao on 2023/2/6.
//

#pragma once

#include <memory>
#include <srs_app_source.hpp>

namespace Tornado {
class RTMPStream {
 public:
  using SourceDeleter = void (*)(SrsLiveSource*);

  explicit RTMPStream(SrsLiveSource* srs_live_source);

 private:
  static void ReleaseSource(SrsLiveSource* srs_live_source);

  struct SrsLiveSourceBridger : public ISrsLiveSourceBridger {
    explicit SrsLiveSourceBridger(RTMPStream* rtmp_stream);

    srs_error_t on_publish() override;
    srs_error_t on_audio(SrsSharedPtrMessage* audio) override;
    srs_error_t on_video(SrsSharedPtrMessage* video) override;
    void on_unpublish() override;

    RTMPStream* rtmp_stream_;
  };

  std::unique_ptr<SrsLiveSource, SourceDeleter> srs_live_source_;
  SrsLiveSourceBridger* live_source_bridger_;
};
}  // namespace Tornado
