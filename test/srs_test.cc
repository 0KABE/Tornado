//
// Created by Chen, WenTao on 2023/1/13.
//

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <chrono>

#include "srs_instance.h"

using namespace std::chrono_literals;
using namespace Tornado;

constexpr static const char* kConfigEnv = "SRS_Config";

TEST(SRS, NotifyOnce) {
  GTEST_SKIP() << "Skip this test case by default";

  const char* config_path = getenv(kConfigEnv);
  ASSERT_NE(config_path, nullptr);

  SRSInstance srs_entrypoint;

  srs_entrypoint.Run(fmt::format("srs -c {}", config_path));

  for (unsigned i = 0; i < 5; ++i) {
    std::this_thread::sleep_for(i * 50ms);
    srs_entrypoint.GetControlCoroutine().NotifyOnce(NotificationEvent::kPlaceholder);
  }

  srs_entrypoint.FastStop();
}
