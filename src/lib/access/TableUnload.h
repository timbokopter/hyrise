// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#ifndef SRC_LIB_ACCESS_TABLEUNLOAD_H_
#define SRC_LIB_ACCESS_TABLEUNLOAD_H_

#include "access/PlanOperation.h"

namespace hyrise {
namespace access {

class TableUnload : public _PlanOperation {
public:
  virtual ~TableUnload();

  void executePlanOperation();
  static std::shared_ptr<_PlanOperation> parse(Json::Value &data);
  const std::string vname();
  void setTableName(const std::string &tablename);

private:
  std::string _table_name;
};

}
}

#endif  // SRC_LIB_ACCESS_TABLEUNLOAD_H_
