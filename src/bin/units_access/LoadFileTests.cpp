// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#include "access/LoadFile.h"
#include "io/shortcuts.h"
#include "testing/test.h"

namespace hyrise {
namespace access {

class LoadFileTests : public AccessTest {};

TEST_F(LoadFileTests, basic_load_file_test) {
  auto t = Loader::shortcuts::load("test/tables/employees.tbl");

  LoadFile lf("tables/employees.tbl");
  lf.execute();

  auto result = lf.getResultTable();

  ASSERT_TRUE(result->contentEquals(t));
}

}
}
