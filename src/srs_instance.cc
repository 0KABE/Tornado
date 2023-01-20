//
// Created by Chen, WenTao on 2023/1/12.
//

#include "srs_instance.h"

#include <spdlog/spdlog.h>

#include <csignal>
#include <exception>
#include <srs_core.hpp>
#include <srs_kernel_error.hpp>
#include <srs_kernel_log.hpp>

#include "entrypoint.h"
#include "final.h"

Tornado::SRSInstance::SRSInstance(int argc, char** argv) {
  srs_error_t err = do_main(argc, argv);
  Final _([&] { srs_freep(err); });

  if (err != srs_success) {
    srs_error("Failed, %s", srs_error_desc(err).c_str());
    throw std::runtime_error("fail to execute do_main");
  }
}

void Tornado::SRSInstance::Run() {
  srs_thread_ = std::thread([] {
    srs_error_t err = srs_success;

    if ((err = run_directly_or_daemon()) != srs_success) {
      throw std::runtime_error(srs_error_summary(err));
    }

    spdlog::info("SRS stopped");
  });
}

void Tornado::SRSInstance::GracefullyStop() {
  spdlog::info("Gracefully stop SRS");
  if (auto err = pthread_kill(srs_thread_.native_handle(), SIGQUIT)) {
    spdlog::error("Gracefully stop failed, error: {}", err);
  }
  if (srs_thread_.joinable()) {
    srs_thread_.join();
    spdlog::info("Complete to stop SRS gracefully");
  }
}

void Tornado::SRSInstance::FastStop() {
  spdlog::info("Fast stop SRS");
  if (auto err = pthread_kill(srs_thread_.native_handle(), SIGTERM)) {
    spdlog::error("Fast stop failed, error: {}", err);
  }
  if (srs_thread_.joinable()) {
    srs_thread_.join();
    spdlog::info("Complete to stop SRS in fast");
  }
}
