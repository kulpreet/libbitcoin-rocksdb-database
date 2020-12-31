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
#include <bitcoin/database/transaction_context.hpp>
#include "rocksdb/db.h"

namespace libbitcoin {
namespace database {

transaction_context::transaction_context(std::shared_ptr<rocksdb::OptimisticTransactionDB> db)
  : db_(db)
{
}

void
transaction_context::begin(const bool use_snapshot)
{
    rocksdb::WriteOptions write_options;
    rocksdb::OptimisticTransactionOptions txn_options;
    if (use_snapshot) {
        txn_options.set_snapshot = true;
    }
    txn_ = std::shared_ptr<rocksdb::Transaction>(db_->BeginTransaction(
          write_options, txn_options));
}

bool
transaction_context::commit()
{
    auto status = txn_->Commit();
    return status.ok();
}

std::shared_ptr<rocksdb::Transaction>
transaction_context::txn() const
{
    return txn_;
}

} // database
} // libbitcoin
