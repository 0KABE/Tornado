//
// Created by Chen, WenTao on 2023/1/13.
//

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <chrono>

#include "args_separator.h"
#include "srs_instance.h"

using namespace std::chrono_literals;

TEST(SRS, StartAndStop) {
  std::string config_env =
      fmt::format("{}_{}_Config", test_info_->test_suite_name(), test_info_->name());

  const char* config_path = getenv(config_env.c_str());
  ASSERT_NE(config_path, nullptr);

  Tornado::ArgsSeparator args_separator(fmt::format("srs -c {}", config_path));
  Tornado::SRSInstance srs_entrypoint(args_separator.GetArgc(), args_separator.GetArgv());

  srs_entrypoint.Run();
  std::this_thread::sleep_for(500ms);
  srs_entrypoint.FastStop();
  //  srs_entrypoint.GracefullyStop();  // SIGQUIT may not work under IDE env
}
