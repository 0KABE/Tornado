//
// Created by Chen, WenTao on 2023/1/12.
//

#include "args_separator.h"

#include <fmt/format.h>
#include <wordexp.h>

#include <limits>

#include "final.h"

namespace Tornado {

ArgsSeparator::ArgsSeparator(const std::string& arg_cmd_line) {
  if (int code = wordexp(arg_cmd_line.c_str(), &word_exp_, 0)) {
    throw std::invalid_argument(fmt::format("fail to parse args, code: {}", code));
  }
}

ArgsSeparator::~ArgsSeparator() { wordfree(&word_exp_); }

ArgsSeparator::Argc ArgsSeparator::GetArgc() const {
  if (word_exp_.we_wordc >= std::numeric_limits<Argc>::max()) {
    throw std::invalid_argument("argc is larger than max of int");
  }
  return static_cast<Argc>(word_exp_.we_wordc);
}

ArgsSeparator::Argv ArgsSeparator::GetArgv() const { return word_exp_.we_wordv; }

ArgsSeparator::Arg ArgsSeparator::operator[](size_t index) const {
  if (index >= GetArgc()) {
    throw std::overflow_error("index is larger than argc");
  }
  return word_exp_.we_wordv[index];
}

}  // namespace Tornado