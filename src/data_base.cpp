/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/database/data_base.hpp>
#include "rocksdb/db.h"

namespace libbitcoin {
namespace database {

using namespace bc::system;
using namespace bc::system::chain;
using namespace bc::system::machine;

data_base::data_base(const path& directory, bool catalog, bool filter)
  : closed_(true), directory_(directory), catalog_(catalog), filter_(filter)
{
}

data_base::~data_base()
{
    close();
}

bool
data_base::create(const system::chain::block& genesis)
{
    rocksdb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = true;
    rocksdb::Status status = rocksdb::OptimisticTransactionDB::Open(options, directory_.string(), &db_);
    BITCOIN_ASSERT_MSG(status.ok(), "Failed to open optimistic transactions database");

    rocksdb::ColumnFamilyHandle *transactions_cf, *blocks_cf;
    status = db_->CreateColumnFamily(rocksdb::ColumnFamilyOptions(),
        TRANSACTIONS_COLUMN_FAMILY, &transactions_cf);
    BITCOIN_ASSERT_MSG(status.ok(), "Failed to create transactions column family");
    column_family_handles_.push_back(transactions_cf);

    status = db_->CreateColumnFamily(rocksdb::ColumnFamilyOptions(),
        BLOCKS_COLUMN_FAMILY, &blocks_cf);
    BITCOIN_ASSERT_MSG(status.ok(), "Failed to create blocks column family");
    column_family_handles_.push_back(blocks_cf);

    closed_ = false;
    return push(genesis) == error::success;
}

bool
data_base::open()
{
    std::vector<rocksdb::ColumnFamilyDescriptor> column_families;
    column_families.push_back(rocksdb::ColumnFamilyDescriptor(
            rocksdb::kDefaultColumnFamilyName, rocksdb::ColumnFamilyOptions()));
    column_families.push_back(rocksdb::ColumnFamilyDescriptor(TRANSACTIONS_COLUMN_FAMILY,
            rocksdb::ColumnFamilyOptions()));
    column_families.push_back(rocksdb::ColumnFamilyDescriptor(BLOCKS_COLUMN_FAMILY,
            rocksdb::ColumnFamilyOptions()));

    rocksdb::Options options;
    rocksdb::Status status = rocksdb::OptimisticTransactionDB::Open(options, directory_.string(),
        column_families, &column_family_handles_, &db_);
    if (!status.ok()) {
        return false;
    }

    transactions_ = std::make_shared<transaction_database>(db_,
        column_family_handles_[1], CACHE_CAPACITY);
    // blocks_ = std::make_shared<blocks_database>(db_,
    //     std::shared_ptr<rocksdb::ColumnFamilyHandle*>(column_family_handles_[2]));

    closed_ = false;
    return true;
}

bool
data_base::close()
{
    if (closed_){
        return false;
    }
    for (auto handle : column_family_handles_) {
        auto s = db_->DestroyColumnFamilyHandle(handle);
        BITCOIN_ASSERT_MSG(s.ok(), "Failed to close rocks db");
    }
    auto status = db_->Close();
    if (!status.ok()){
        return false;
    }
    closed_ = true;
    return true;
}

system::code
data_base::push(const system::chain::block& block, size_t height,
    uint32_t median_time_past)
{
    // start rocksdb batch as RAII. So it transaction commit on
    // function exit.
    // 1. store block header
    // 2. mark block as confirmed, index in rockdb?
    // 3. store all block transactions
    // 4. link block transaction to block as it is a confirmed block
    // 5. Confirm all transactions. State transition via candidate is
    // not required.
    // 6. Mark block as valid, as it is presumed valid.
    // 7. mark block as valid, index rockdb?
    return error::success;
}

} // namespace database
} // namespace libbitcoin
