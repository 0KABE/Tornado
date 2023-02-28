//
// Created by Chen, WenTao on 2023/2/6.
//

#include "rtmp_stream.h"

#include <spdlog/spdlog.h>

#include "base.h"

Tornado::RTMPStream::RTMPStream(SrsLiveSource *srs_live_source)
    : srs_live_source_(srs_live_source, &ReleaseSource) {
  spdlog::debug("srs_live_source_: {}, RTMPStream: {}", fmt::ptr(srs_live_source), fmt::ptr(this));
}

// TODO: ReleaseSource should make sure that be executed in SRS thread, or there is condition racing
void Tornado::RTMPStream::ReleaseSource(SrsLiveSource *srs_live_source) {
  srs_live_source->set_bridger(nullptr);
  srs_live_source->on_unpublish();
  srs_live_source->dispose();
}

asio::awaitable<Tornado::VideoMessage> Tornado::RTMPStream::AsyncFetchVideoMessage() {
  auto executor = co_await asio::this_coro::executor;
  {
    std::lock_guard _(mutex_);
    if (!video_waiter_) {
      video_waiter_ = std::make_shared<Event>(executor);
    }
  }

  co_await video_waiter_->AsyncWait();
  VideoMessage video_message = co_await AsyncFetchVideoMessageInternal();
  co_return video_message;
}

void Tornado::RTMPStream::RegisterBridge() {
  live_source_bridger_ = new SrsLiveSourceBridger(shared_from_this());
  srs_live_source_->set_bridger(live_source_bridger_);
}

asio::awaitable<Tornado::VideoMessage> Tornado::RTMPStream::AsyncFetchVideoMessageInternal() {
  return asio::async_initiate<UseAwaitable, void(VideoMessage)>(
      [self = shared_from_this()](auto handler) {
        spdlog::trace("AsyncFetchVideoMessageInternal start");

        VideoMessage video_message;
        {
          std::lock_guard _(self->mutex_);
          video_message = VideoMessage(
              self->video_message_->size,
              reinterpret_cast<VideoMessage::PayloadItem *>(self->video_message_->payload));
        }

        handler(std::move(video_message));
        spdlog::trace("AsyncFetchVideoMessageInternal finish");
      },
      asio::use_awaitable);
}

srs_error_t Tornado::RTMPStream::SrsLiveSourceBridger::on_publish() { return nullptr; }

srs_error_t Tornado::RTMPStream::SrsLiveSourceBridger::on_audio(SrsSharedPtrMessage *audio) {
  spdlog::trace("SrsLiveSourceBridger::on_audio");
  std::lock_guard _(rtmp_stream_->mutex_);

  if (rtmp_stream_->audio_waiter_) {
    spdlog::debug("received audio frame from SRS");
    rtmp_stream_->audio_message_ = *audio;
    rtmp_stream_->audio_waiter_->NotifyOnce();
  } else {
    spdlog::debug("received audio frame from SRS, but no customer. skip it");
  }

  return srs_success;
}

srs_error_t Tornado::RTMPStream::SrsLiveSourceBridger::on_video(SrsSharedPtrMessage *video) {
  spdlog::trace("SrsLiveSourceBridger::on_video");
  std::lock_guard _(rtmp_stream_->mutex_);

  if (rtmp_stream_->video_waiter_) {
    spdlog::debug("received video frame from SRS");
    rtmp_stream_->video_message_ = *video;
    rtmp_stream_->video_waiter_->NotifyOnce();
  } else {
    spdlog::debug("received video frame from SRS, but no customer. skip it");
  }

  return srs_success;
}

void Tornado::RTMPStream::SrsLiveSourceBridger::on_unpublish() {}
