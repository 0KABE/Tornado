//
// Created by Chen, WenTao on 2023/1/12.
//

#pragma once

#include <functional>

namespace Tornado {
class Final {
 public:
  explicit Final(std::function<void()> action);
  ~Final();

  Final(Final&&) = delete;
  Final(const Final&) = delete;
  Final& operator=(Final&&) = delete;
  Final& operator=(const Final&) = delete;

 private:
  std::function<void()> action_;
};
}  // namespace Tornado
