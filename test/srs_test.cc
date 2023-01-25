//
// Created by Chen, WenTao on 2023/1/13.
//

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <chrono>

#include "srs_instance.h"

using namespace std::chrono_literals;

TEST(SRS, StartAndStop) {
  std::string config_env =
      fmt::format("{}_{}_Config", test_info_->test_suite_name(), test_info_->name());

  const char* config_path = getenv(config_env.c_str());
  ASSERT_NE(config_path, nullptr);

  Tornado::SRSInstance srs_entrypoint;

  srs_entrypoint.Run(fmt::format("srs -c {}", config_path));
  std::this_thread::sleep_for(100ms);
  srs_entrypoint.FastStop();
  //  srs_entrypoint.GracefullyStop();  // SIGQUIT may not work under IDE env
}
