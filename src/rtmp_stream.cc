//
// Created by Chen, WenTao on 2023/2/6.
//

#include "rtmp_stream.h"

#include <spdlog/spdlog.h>

Tornado::RTMPStream::RTMPStream(SrsLiveSource *srs_live_source)
    : srs_live_source_(srs_live_source, &ReleaseSource) {
  spdlog::debug("srs_live_source_: {}, RTMPStream: {}", fmt::ptr(srs_live_source), fmt::ptr(this));
  srs_live_source_->set_bridger(this);
}

// TODO: ReleaseSource should make sure that be executed in SRS thread, or there is condition racing
void Tornado::RTMPStream::ReleaseSource(SrsLiveSource *srs_live_source) {
  srs_live_source->set_bridger(nullptr);
  srs_live_source->on_unpublish();
}

srs_error_t Tornado::RTMPStream::on_publish() { return nullptr; }

srs_error_t Tornado::RTMPStream::on_audio(SrsSharedPtrMessage *audio) {
  spdlog::debug("received audio frame from SRS");
  return srs_success;
}

srs_error_t Tornado::RTMPStream::on_video(SrsSharedPtrMessage *video) {
  spdlog::debug("received video frame from SRS");
  return srs_success;
}

void Tornado::RTMPStream::on_unpublish() {}
