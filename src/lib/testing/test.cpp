// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#include "testing/test.h"

#include <storage/PrettyPrinter.h>

::testing::AssertionResult AssertTableContentEquals(const char *left_exp,
                                                    const char *right_exp,
                                                    const hyrise::storage::c_atable_ptr_t left,
                                                    const hyrise::storage::c_atable_ptr_t right) {
  if (left->contentEquals(right)) {
    return ::testing::AssertionSuccess();
  }

  std::stringstream buf;

  PrettyPrinter::print(left.get(), buf, left_exp);
  PrettyPrinter::print(right.get(), buf, right_exp);
  return ::testing::AssertionFailure() << buf.str()
                                       << "The content of " << left_exp << " does not equal the content of " << right_exp;
}
