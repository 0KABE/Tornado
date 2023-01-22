//
// Created by Chen, WenTao on 2023/1/12.
//

#pragma once

#include <thread>

namespace Tornado {

class SRSInstance {
 public:
  SRSInstance(int argc, char** argv);

  void Run();

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

}  // namespace Tornado
