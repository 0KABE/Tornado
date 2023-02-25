//
// Created by Chen, WenTao on 2023/2/6.
//

#include "rtmp_stream.h"

#include <spdlog/spdlog.h>

Tornado::RTMPStream::RTMPStream(SrsLiveSource *srs_live_source)
    : srs_live_source_(srs_live_source, &ReleaseSource),
      live_source_bridger_(new SrsLiveSourceBridger(this)) {
  spdlog::debug("srs_live_source_: {}, RTMPStream: {}", fmt::ptr(srs_live_source), fmt::ptr(this));
  srs_live_source_->set_bridger(live_source_bridger_);
}

// TODO: ReleaseSource should make sure that be executed in SRS thread, or there is condition racing
void Tornado::RTMPStream::ReleaseSource(SrsLiveSource *srs_live_source) {
  srs_live_source->set_bridger(nullptr);
  srs_live_source->on_unpublish();
}

srs_error_t Tornado::RTMPStream::SrsLiveSourceBridger::on_publish() { return nullptr; }

srs_error_t Tornado::RTMPStream::SrsLiveSourceBridger::on_audio(SrsSharedPtrMessage *audio) {
  spdlog::debug("received audio frame from SRS");
  return srs_success;
}

srs_error_t Tornado::RTMPStream::SrsLiveSourceBridger::on_video(SrsSharedPtrMessage *video) {
  spdlog::debug("received video frame from SRS");
  return srs_success;
}

void Tornado::RTMPStream::SrsLiveSourceBridger::on_unpublish() {}

Tornado::RTMPStream::SrsLiveSourceBridger::SrsLiveSourceBridger(Tornado::RTMPStream *rtmp_stream)
    : rtmp_stream_(rtmp_stream) {}
