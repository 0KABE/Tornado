//
// Created by Chen, WenTao on 2023/1/10.
//
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);

  if (const char* log_level = getenv("LOG_LEVEL")) {
    spdlog::set_level(spdlog::level::from_str(log_level));
    spdlog::set_pattern("[%T.%e] [%L] [%t] %v");
  }

  return RUN_ALL_TESTS();
}
