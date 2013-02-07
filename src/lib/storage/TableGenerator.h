// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#ifndef SRC_LIB_STORAGE_TABLEGENERATOR_H_
#define SRC_LIB_STORAGE_TABLEGENERATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <vector>
#include <string>
#include <set>

#include <boost/random.hpp>

#include "AbstractTable.h"

class TableGenerator {
 public:

  explicit TableGenerator(bool quiet = false);
  ~TableGenerator();

  /*
    Prepare the current instance to handle a certain amount of
    data. This generates a one time cost, but will speedup
    generation fo subsequent tables.
  */
  void prepare(size_t p);

  AbstractTable::SharedTablePtr int_random(size_t rows,
                                           size_t cols,
                                           size_t mod = 0,
                                           std::vector<unsigned> layout = std::vector<unsigned>());

  AbstractTable::SharedTablePtr int_random_delta(size_t rows,
                                                 size_t cols,
                                                 size_t mod = 0,
                                                 std::vector<unsigned> layout = std::vector<unsigned>());

  AbstractTable::SharedTablePtr int_offset(size_t rows, size_t cols, size_t offset1, size_t offset2, size_t offset2_start, size_t factor, int big_value_at_end = -1);

  AbstractTable::SharedTablePtr int_offset_delta(size_t rows, size_t cols, size_t offset1, size_t offset2, size_t offset2_start, size_t factor, int big_value_at_end = -1);

  AbstractTable::SharedTablePtr one_value(size_t rows, size_t cols, int value);

  AbstractTable::SharedTablePtr one_value_delta(size_t rows, size_t cols, int value);

  AbstractTable::SharedTablePtr int_distinct(size_t row_count, size_t column_count, size_t distinct_count);

  AbstractTable::SharedTablePtr create_empty_table(size_t rows, std::vector<std::string> containers = std::vector<std::string>());
  AbstractTable::SharedTablePtr create_empty_table(size_t rows, size_t cols, std::vector<unsigned> containers = std::vector<unsigned>());

  AbstractTable::SharedTablePtr create_empty_table_modifiable(size_t rows, size_t cols, std::vector<std::string> name = std::vector<std::string>());

  template<typename Strategy>
  AbstractTable::SharedTablePtr create_empty_base_table_modifiable(size_t rows, size_t cols);

  std::vector<AbstractTable::SharedTablePtr > distinct_cols(size_t cols, size_t main_size, size_t delta_size);

  std::vector<int64_t> *create_dicts(size_t dict_size_main, size_t dict_size_delta, size_t intersection, size_t tail);

  std::vector<AbstractTable::SharedTablePtr > one_column_main_delta(size_t rows_main, size_t rows_delta, size_t dict_size_main, size_t dict_size_delta, size_t intersection, size_t tail);

  std::vector<AbstractTable::SharedTablePtr > value_order_successively(size_t rows_main, size_t rows_delta, size_t dict_size_main, size_t dict_size_delta, size_t intersection, size_t tail);

  std::string random_string(const int len);
  AbstractTable::SharedTablePtr string_random(size_t rows, size_t cols, int string_length);
  AbstractTable::SharedTablePtr string_random_delta(size_t rows, size_t cols, int string_length);

  int selfsimilar(int64_t n, double h);
  AbstractTable::SharedTablePtr int_random_weighted(size_t rows, size_t cols, size_t n, size_t h);
  AbstractTable::SharedTablePtr int_random_weighted_delta(size_t rows, size_t cols, size_t n, size_t h);

 protected:
  bool _quiet;
  size_t _total;
  size_t _current;
  size_t _current_step;
  size_t _steps;

  // We will generate a certain size of random integeres to avoid
  // later cost
  size_t _prepareSize;

  // Stores the prepared values
  std::set<int64_t> _values;

  boost::mt19937 gen;

  void start(size_t rows, size_t cols, size_t total);
  void increment();
};

#include "TableGenerator-impl.h"

#endif  // SRC_LIB_STORAGE_TABLEGENERATOR_H_