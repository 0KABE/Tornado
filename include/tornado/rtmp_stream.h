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

  std::unique_ptr<SrsLiveSource, SourceDeleter> srs_live_source_;
};
}  // namespace Tornado
