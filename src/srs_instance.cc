//
// Created by Chen, WenTao on 2023/1/12.
//

#include "srs_instance.h"

#include <spdlog/spdlog.h>

#include <csignal>
#include <exception>
#include <srs_app_st.hpp>
#include <srs_core.hpp>
#include <srs_kernel_error.hpp>
#include <srs_kernel_log.hpp>
#include <srs_service_st.hpp>

#include "args_separator.h"
#include "entrypoint.h"
#include "final.h"

static std::optional<Tornado::TornadoCoroutine> tornado_coroutine_;

void Tornado::SRSInstance::Run(const std::string& arguments) {
  srs_thread_ = std::thread([this, arguments] {
    Tornado::ArgsSeparator args_separator(arguments);

    srs_error_t err = srs_success;
    Final _([&] { srs_freep(err); });

    err = do_main(args_separator.GetArgc(), args_separator.GetArgv());
    tornado_coroutine_ = TornadoCoroutine();

    if (err != srs_success) {
      srs_error("Failed, %s", srs_error_desc(err).c_str());
      throw std::runtime_error("fail to execute do_main");
    }

    tornado_coroutine_.value().Start();

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

// const Tornado::TornadoCoroutine& Tornado::SRSInstance::GetControlCoroutine() const {
//   return tornado_coroutine_.value();
// }

Tornado::TornadoCoroutine::TornadoCoroutine()
    : coroutine_(new SrsSTCoroutine(kCoroutineName, this, _srs_context->get_id())) {
  spdlog::info("TornadoCoroutine::TornadoCoroutine()");
}

srs_error_t Tornado::TornadoCoroutine::Start() {
  srs_error_t err = srs_success;

  //  if (pipe(pipe_.data()) < 0) {
  //    return srs_error_new(-1, "Create pipe failed: %s(%d)", strerror(errno), errno);
  //  }

  //  if ((read_fd_ = srs_netfd_open(pipe_[1])) == nullptr) {
  //    return srs_error_new(ERROR_SYSTEM_CREATE_PIPE, "open pipe");
  //  }

  if ((err = coroutine_->start()) != srs_success) {
    return srs_error_wrap(err, "start timer");
  }

  return err;
}

// void Tornado::TornadoCoroutine::NotifyOnce(Tornado::NotificationEvent notification_event) const {
//   write(pipe_[0], &notification_event, sizeof(notification_event));
// }
//
srs_error_t Tornado::TornadoCoroutine::cycle() {
  // TODO:
  //  1. startup coroutine in SRS
  //  2. use st_mutex in signal handler & cycle
  srs_error_t err = srs_success;

  for (;;) {
    spdlog::info("st_coroutine: {}", fmt::ptr(coroutine_.get()));
    if ((err = coroutine_->pull()) != srs_success) {
      return srs_error_wrap(err, "quit");
    }

    //    NotificationEvent notification_event;
    //    srs_read(read_fd_, &notification_event, sizeof(notification_event), SRS_UTIME_NO_TIMEOUT);
    spdlog::info("Tornado::TornadoCoroutine::cycle()");
    //    srs_usleep(100 * SRS_UTIME_MILLISECONDS);
  }

  return err;
}
