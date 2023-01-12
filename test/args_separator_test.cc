//
// Created by Chen, WenTao on 2023/1/12.
//

#include "args_separator.h"

#include <gtest/gtest.h>

using Tornado::ArgsSeparator;

TEST(ArgsSeparator, Case1) {
  ArgsSeparator args("-a -b -c dog \"-d fire\"");

  EXPECT_EQ(args.GetArgc(), 5);
  EXPECT_STREQ(args[0], "-a");
  EXPECT_STREQ(args[1], "-b");
  EXPECT_STREQ(args[2], "-c");
  EXPECT_STREQ(args[3], "dog");
  EXPECT_STREQ(args[4], "-d fire");

  ArgsSeparator::Argv argv = nullptr;
  ASSERT_NO_THROW(argv = args.GetArgv());
  EXPECT_STREQ(args[0], *argv);

  EXPECT_THROW(args[5], std::overflow_error);
  EXPECT_THROW(ArgsSeparator("\"test"), std::invalid_argument);
}
