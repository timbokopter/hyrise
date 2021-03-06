// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#include "storage/PointerCalculator.h"

#include <iostream>
#include <string>

#include "storage/PrettyPrinter.h"
#include "storage/Store.h"



PointerCalculator::PointerCalculator(hyrise::storage::c_atable_ptr_t t, pos_list_t *pos, field_list_t *f) : table(t), pos_list(pos), fields(f) {
  // prevent nested pos_list/fields: if the input table is a
  // PointerCalculator instance, combine the old and new
  // pos_list/fields lists
  auto p = std::dynamic_pointer_cast<const PointerCalculator>(t);
  if (p) {
    if (pos_list != nullptr && p->pos_list != NULL) {
      pos_list = new pos_list_t(pos->size());
      for (size_t i = 0; i < pos->size(); i++) {
        (*pos_list)[i] = p->pos_list->at(pos->at(i));
      }
      table = p->table;
      delete pos;
    }
    if (fields != nullptr && p->fields != NULL) {
      fields = new field_list_t(f->size());
      for (size_t i = 0; i < f->size(); i++) {
        (*fields)[i] = p->fields->at(f->at(i));
      }
      table = p->table;
    }
  }
  updateFieldMapping();
}

hyrise::storage::atable_ptr_t PointerCalculator::copy() const {
  return std::make_shared<PointerCalculator>(table, fields, pos_list);
}

PointerCalculator::~PointerCalculator() {
  delete fields;
  delete pos_list;
}

void PointerCalculator::updateFieldMapping() {
  slice_for_slice.clear();
  offset_in_slice.clear();
  width_for_slice.clear();
  slice_count = 0;

  size_t field = 0, dst_field = 0;
  size_t last_field = 0;
  bool last_field_set = false;

  for (size_t src_slice = 0; src_slice < table->sliceCount(); src_slice++) {
    last_field_set = false;
    for (size_t src_field = 0; src_field < table->getSliceWidth(src_slice) / sizeof(value_id_t); src_field++) {
      // Check if we have to increase the fields until we reach
      // a projected attribute
      if (fields && (fields->size() <= dst_field || fields->at(dst_field) != field)) {
        field++;
        continue;
      }

      if (!last_field_set || field > last_field + 1) { // new slice
        slice_count++;
        slice_for_slice.push_back(src_slice);
        offset_in_slice.push_back(src_field);
        width_for_slice.push_back(sizeof(value_id_t));
      } else {
        width_for_slice[slice_count - 1] += sizeof(value_id_t);
      }

      dst_field++;
      last_field = field;

      if (!last_field_set) {
        last_field_set = true;
      }

      field++;
    }
  }
}

void PointerCalculator::setPositions(const pos_list_t pos) {
  if (pos_list != nullptr)
    delete pos_list;
  pos_list = new std::vector<pos_t>(pos);
}

void PointerCalculator::setFields(const field_list_t f) {
  fields = new std::vector<field_t>(f);
  updateFieldMapping();
}

const ColumnMetadata *PointerCalculator::metadataAt(const size_t column_index, const size_t row_index, const table_id_t table_id) const {
  size_t actual_column;

  if (fields) {
    actual_column = fields->at(column_index);
  } else {
    actual_column = column_index;
  }

  return table->metadataAt(actual_column);
}

void PointerCalculator::setDictionaryAt(AbstractTable::SharedDictionaryPtr dict, const size_t column, const size_t row, const table_id_t table_id) {
  throw std::runtime_error("Can't set PointerCalculator dictionary");
}

const AbstractTable::SharedDictionaryPtr& PointerCalculator::dictionaryAt(const size_t column, const size_t row, const table_id_t table_id, const bool of_delta) const {
  size_t actual_column, actual_row;

  if (fields) {
    actual_column = fields->at(column);
  } else {
    actual_column = column;
  }

  if (pos_list && pos_list->size() > 0) {
    actual_row = pos_list->at(row);
  } else {
    actual_row = row;
  }

  return table->dictionaryAt(actual_column, actual_row, table_id);
}

const AbstractTable::SharedDictionaryPtr& PointerCalculator::dictionaryByTableId(const size_t column, const table_id_t table_id) const {
  size_t actual_column;

  if (fields) {
    actual_column = fields->at(column);
  } else {
    actual_column = column;
  }

  return table->dictionaryByTableId(actual_column, table_id);
}

size_t PointerCalculator::size() const {
  if (pos_list) {
    return pos_list->size();
  }

  return table->size();
}

size_t PointerCalculator::columnCount() const {
  if (fields) {
    return fields->size();
  }

  return table->columnCount();
}

ValueId PointerCalculator::getValueId(const size_t column, const size_t row) const {
  size_t actual_column, actual_row;

  if (pos_list) {
    actual_row = pos_list->at(row);
  } else {
    actual_row = row;
  }

  if (fields) {
    actual_column = fields->at(column);
  } else {
    actual_column = column;
  }

  return table->getValueId(actual_column, actual_row);
}

unsigned PointerCalculator::sliceCount() const {
  return slice_count;
}

void *PointerCalculator::atSlice(const size_t slice, const size_t row) const {
  size_t actual_row;

  if (pos_list) {
    actual_row = pos_list->at(row);
  } else {
    actual_row = row;
  }

  if (fields) {
    void *at = table->atSlice(slice_for_slice[slice], row);
    at = (value_id_t *)at + offset_in_slice[slice];
    return at;
  }

  return table->atSlice(slice, actual_row);
}

size_t PointerCalculator::getSliceWidth(const size_t slice) const {
  // FIXME this should return the width in bytes for the column
  if (fields) {
    return width_for_slice[slice];
  }

  return table->getSliceWidth(slice);
}


size_t PointerCalculator::getSliceForColumn(const size_t column) const {
  return 0;
};

size_t PointerCalculator::getOffsetInSlice(const size_t column) const {
  return 0;
};

void PointerCalculator::print(const size_t limit) const {
  PrettyPrinter::print(this, std::cout, "unnamed pointer calculator", limit);
}

std::string PointerCalculator::printValue(const size_t column, const size_t row) const {
  size_t actual_column, actual_row;

  if (pos_list) {
    actual_row = pos_list->at(row);
  } else {
    actual_row = row;
  }

  if (fields) {
    actual_column = fields->at(column);
  } else {
    actual_column = column;
  }

  return table->printValue(actual_column, actual_row);
}

void PointerCalculator::sortDictionary() {
  throw std::runtime_error("Can't sort PointerCalculator dictionary");
}

size_t PointerCalculator::getTableRowForRow(const size_t row) const
{
  size_t actual_row;
  // resolve mapping of THIS pointer calculator
  if (pos_list) {
      actual_row = pos_list->at(row);
  } else {
      actual_row = row;
  }
  // if underlying table is PointerCalculator, resolve recursively
  auto p = std::dynamic_pointer_cast<const PointerCalculator>(table);
  if (p)
    actual_row = p->getTableRowForRow(actual_row);

  return actual_row;
}

size_t PointerCalculator::getTableColumnForColumn(const size_t column) const
{
  size_t actual_column;
  // resolve field mapping of THIS pointer calculator
  if (fields) {
    actual_column = fields->at(column);
  } else {
    actual_column = column;
  }
  // if underlying table is PointerCalculator, resolve recursively
  auto p = std::dynamic_pointer_cast<const PointerCalculator>(table);
  if (p)
    actual_column = p->getTableColumnForColumn(actual_column);
  return actual_column;
}

hyrise::storage::c_atable_ptr_t PointerCalculator::getTable() const {
  return table;
}

hyrise::storage::c_atable_ptr_t PointerCalculator::getActualTable() const {
  auto p = std::dynamic_pointer_cast<const PointerCalculator>(table);

  if (!p) {
    return table;
  } else {
    return p->getActualTable();
  }
}

const pos_list_t *PointerCalculator::getPositions() const {
  return pos_list;
}

pos_list_t PointerCalculator::getActualTablePositions() const {
  auto p = std::dynamic_pointer_cast<const PointerCalculator>(table);

  if (!p) {
    return *pos_list;
  }

  pos_list_t result(pos_list->size());
  pos_list_t positions = p->getActualTablePositions();

  for (pos_list_t::const_iterator it = pos_list->begin(); it != pos_list->end(); ++it) {
    result.push_back(positions[*it]);
  }

  return result;
}


//FIXME: Template this method
hyrise::storage::atable_ptr_t PointerCalculator::copy_structure(const field_list_t *fields, const bool reuse_dict, const size_t initial_size, const bool with_containers) const {
  std::vector<const ColumnMetadata *> metadata;
  std::vector<AbstractTable::SharedDictionaryPtr> *dictionaries = nullptr;

  if (reuse_dict) {
    dictionaries = new std::vector<AbstractTable::SharedDictionaryPtr>();
  }

  if (fields != nullptr) {
for (const field_t & field: *fields) {
      metadata.push_back(metadataAt(field));

      if (dictionaries != nullptr) {
        dictionaries->push_back(dictionaryAt(field, 0, 0, true));
      }
    }
  } else {
    for (size_t i = 0; i < columnCount(); ++i) {
      metadata.push_back(metadataAt(i));

      if (dictionaries != nullptr) {
        dictionaries->push_back(dictionaryAt(i, 0, 0, true));
      }
    }
  }

  auto result = std::make_shared<Table<>>(&metadata, dictionaries, initial_size);
  delete dictionaries;
  return result;
}


std::shared_ptr<PointerCalculator> PointerCalculator::intersect(const std::shared_ptr<const PointerCalculator>& other) const {
  pos_list_t *result = new pos_list_t(std::max(pos_list->size(), other->pos_list->size()));

  std::set_intersection(pos_list->begin(), pos_list->end(),
                        other->pos_list->begin(), other->pos_list->end(),
                        std::back_inserter(*result));

  assert((other->table == this->table) && "Should point to same table");
  return std::make_shared<PointerCalculator>(table, result, fields);  
}

