// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#include "storage/TableBuilder.h"

#include "storage/AbstractTable.h"
#include "storage/Table.h"
#include "storage/MutableVerticalTable.h"

void TableBuilder::checkParams(const param_list &args) {
  if (args.size() == 0)
    throw TableBuilderError("Cannot build a table with no columns");

  // TODO check if the number of columns in groups is correct
  size_t sum = 0;
for (const auto & i: args.groups())
    sum += i;

  if (sum != args.size())
    throw TableBuilderError("Specified Layout does not match number of columns");
}

hyrise::storage::atable_ptr_t TableBuilder::createTable(param_list::param_list_t::const_iterator begin,
    param_list::param_list_t::const_iterator end,
    const bool compressed) {
  // Meta data container
  std::vector<const ColumnMetadata *> vc;
  std::vector<AbstractTable::SharedDictionaryPtr > vd;

  for (; begin != end; ++begin) {
    vc.push_back(ColumnMetadata::metadataFromString((*begin).type, (*begin).name));
    vd.push_back(
      DictionaryFactory<OrderIndifferentDictionary>::build(vc.back()->getType()));
  }

  auto tmp = std::make_shared<Table<>>(&vc, &vd, 0, 0, 0, 64, compressed);

for (const auto & column_meta: vc)
    delete column_meta;

  return tmp;
}

hyrise::storage::atable_ptr_t TableBuilder::build(param_list args, const bool compressed) {
  if (args.groups().size() == 0)
    args.appendGroup(args.size());

  checkParams(args);

  std::vector<hyrise::storage::atable_ptr_t> base;
  size_t begin, end;
  begin = 0;
  for (size_t g = 0; g < args.groups().size(); ++g) {
    // Calculate the upper bound for the current layout
    end = args.groups().size() > g + 1 ? args.groups()[g + 1] : args.size();
    base.push_back(createTable(args.params().begin() + begin, args.params().begin() + end, compressed));
    begin = end;
  }

  return std::move(std::make_shared<MutableVerticalTable>(base));
}
