//
// Created by Chen, WenTao on 2023/1/12.
//

#pragma once

#include <wordexp.h>

#include <string>
#include <vector>

namespace Tornado {

class ArgsSeparator {
 public:
  using Arg = char*;
  using Argc = int;
  using Argv = Arg*;

  explicit ArgsSeparator(const std::string& args);

  ~ArgsSeparator();

  [[nodiscard]] Argc GetArgc() const;

  [[nodiscard]] Argv GetArgv() const;

  Arg operator[](size_t index) const;

 private:
  wordexp_t word_exp_{};
};

}  // namespace Tornado
