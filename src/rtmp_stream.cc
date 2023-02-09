//
// Created by Chen, WenTao on 2023/2/6.
//

#include "rtmp_stream.h"

Tornado::RTMPStream::RTMPStream(SrsLiveSource *srs_live_source)
    : srs_live_source_(srs_live_source, &ReleaseSource) {}

void Tornado::RTMPStream::ReleaseSource(SrsLiveSource *srs_live_source) {
  srs_live_source->on_unpublish();
}
