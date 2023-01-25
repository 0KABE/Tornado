//
// Created by Chen, WenTao on 2023/1/12.
//

#pragma once

#include <srs_app_st.hpp>
#include <thread>

namespace Tornado {

class SRSInstance {
 public:
  SRSInstance() = default;

  void Run(const std::string& arguments);

  /**
   * Like as gracefully quit in SRS, thread will not stop until there are no connection
   */
  void GracefullyStop();

  /**
   * Like as fast quit in SRS, thread will stop quickly.
   *
   * @note The behavior can be modified to force gracefully quit in config
   */
  void FastStop();

  ~SRSInstance() = default;

 private:
  std::thread srs_thread_;
};

class TornadoCoroutine : public ISrsCoroutineHandler {
 public:
  constexpr static const char* kCoroutineName = "TornadoCoroutine";

  TornadoCoroutine();

  srs_error_t Start();
  void NotifyOnce();

 private:
  srs_error_t cycle() override;

  std::unique_ptr<SrsSTCoroutine> st_coroutine_;
};

}  // namespace Tornado
