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
  void GracefullyStop();
  void FastStop();

  ~SRSInstance() = default;

 private:
  std::thread srs_thread_;
};

}  // namespace Tornado
