//
// Created by Chen, WenTao on 2023/1/12.
//

#include "srs_instance.h"

#include <spdlog/spdlog.h>

#include <csignal>
#include <exception>
#include <latch>
#include <srs_app_st.hpp>
#include <srs_core.hpp>
#include <srs_kernel_error.hpp>
#include <srs_kernel_log.hpp>
#include <srs_service_st.hpp>

#include "args_separator.h"
#include "entrypoint.h"
#include "final.h"

void Tornado::SRSInstance::Run(const std::string& arguments) {
  std::latch latch(1);
  srs_thread_ = std::thread([this, arguments, &latch] {
    Tornado::ArgsSeparator args_separator(arguments);

    srs_error_t err = srs_success;
    Final _([&] { srs_freep(err); });

    err = do_main(args_separator.GetArgc(), args_separator.GetArgv());

    if (err != srs_success) {
      srs_error("Failed, %s", srs_error_desc(err).c_str());
      throw std::runtime_error("fail to execute do_main");
    }

    // The constructor will pass the pointer to itself
    // If the object is constructed in stack, it will be failed to access in another st coroutine
    // which causes SIGSEGV in running
    tornado_coroutine_ = std::make_unique<TornadoCoroutine>();
    tornado_coroutine_->Start();
    latch.count_down();

    if ((err = run_directly_or_daemon()) != srs_success) {
      throw std::runtime_error(srs_error_summary(err));
    }

    spdlog::info("SRS stopped");
  });
  latch.wait();
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

const Tornado::TornadoCoroutine& Tornado::SRSInstance::GetControlCoroutine() const {
  return *tornado_coroutine_;
}

Tornado::TornadoCoroutine::TornadoCoroutine()
    : coroutine_(new SrsSTCoroutine(kCoroutineName, this, _srs_context->get_id())) {
  spdlog::info("TornadoCoroutine::TornadoCoroutine()");
}

srs_error_t Tornado::TornadoCoroutine::Start() {
  srs_error_t err = srs_success;

  if (pipe(pipe_.data()) < 0) {
    return srs_error_new(-1, "Create pipe failed: %s(%d)", strerror(errno), errno);
  }

  if ((read_fd_ = srs_netfd_open(pipe_[0])) == nullptr) {
    return srs_error_new(ERROR_SYSTEM_CREATE_PIPE, "open pipe");
  }

  if ((err = coroutine_->start()) != srs_success) {
    return srs_error_wrap(err, "start timer");
  }

  return err;
}

void Tornado::TornadoCoroutine::NotifyOnce(Tornado::NotificationEvent notification_event) const {
  if (int ret = write(pipe_[1], &notification_event, sizeof(notification_event));
      ret < sizeof(notification_event)) {
    spdlog::info("write notification event failed, write() returns {}, {}({})", ret, errno,
                 strerror(errno));
    throw std::runtime_error("fail to write into pipe");
  }
  spdlog::info("write notification event");
}

srs_error_t Tornado::TornadoCoroutine::cycle() {
  // TODO:
  //  1. startup coroutine in SRS
  //  2. use st_mutex in signal handler & cycle
  srs_error_t err = srs_success;

  for (;;) {
    if ((err = coroutine_->pull()) != srs_success) {
      return srs_error_wrap(err, "quit");
    }

    NotificationEvent notification_event;
    srs_read(read_fd_, &notification_event, sizeof(notification_event), SRS_UTIME_NO_TIMEOUT);
    spdlog::info("read notification event");
    //    srs_usleep(100 * SRS_UTIME_MILLISECONDS);
  }

  return err;
}
