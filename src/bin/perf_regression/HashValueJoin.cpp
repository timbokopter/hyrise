// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#include <gtest/gtest-bench.h>
#include <gtest/gtest.h>
#include <string>

#include <access.h>
#include <storage.h>
#include <io.h>

//HashValueJoin Benchmark similar to TPC-C Implementation of Stock-Level Transaction
//See TPC-C Reference Chapter A.5

class HashValueJoinBase : public ::testing::Benchmark {

 protected:

  StorageManager *sm;
  hyrise::access::HashValueJoin<int> *hs;
  hyrise::storage::c_atable_ptr_t t1;
  hyrise::storage::c_atable_ptr_t t2;

 public:
  void BenchmarkSetUp() {
    hs = new hyrise::access::HashValueJoin<int>();
    hs->setEvent("NO_PAPI");

    sm = StorageManager::getInstance();

    hs->setProducesPositions(true);

    t1 = sm->getTable("stock");
    t2 = sm->getTable("order_line");
  }

  void BenchmarkTearDown() {
  }

  HashValueJoinBase() {
    SetNumIterations(10);
    SetWarmUp(2);
  }
};


BENCHMARK_F(HashValueJoinBase, stock_level_hash_value_join) {
  hs->addInput(t1);
  hs->addField(0);
  hs->addInput(t2);
  hs->addField(4);

  /*const auto& result =*/ hs->execute()->getResultTable();
}

BENCHMARK_F(HashValueJoinBase, stock_level_hash_value_join_mat) {
  hs->addInput(t1);
  hs->addField(0);
  hs->addInput(t2);
  hs->addField(4);

  auto result = hs->execute()->getResultTable();

  MaterializingScan ms(false);
  ms.setEvent("NO_PAPI");
  ms.addInput(result);

  /*const auto& result_mat =*/ ms.execute()->getResultTable();
}

BENCHMARK_F(HashValueJoinBase, stock_level_hash_value_join_mat_memcpy) {
  hs->addInput(t1);
  hs->addField(0);
  hs->addInput(t2);
  hs->addField(4);

  auto result = hs->execute()->getResultTable();

  MaterializingScan ms(true);
  ms.setEvent("NO_PAPI");
  ms.addInput(result);

  /*const auto& result_mat =*/ ms.execute()->getResultTable();
}
