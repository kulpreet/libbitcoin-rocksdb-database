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
#ifndef LIBBITCOIN_ROCKSDB_DATABASE_ROCKSDB_TXN_CONTEXT_HPP
#define LIBBITCOIN_ROCKSDB_DATABASE_ROCKSDB_TXN_CONTEXT_HPP

#include "rocksdb/db.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/optimistic_transaction_db.h"

namespace libbitcoin {
namespace database {

/// Requires explicit begin and commit to allow use across blocks that
/// don't allow RAII
class transaction_context
{
public:
    transaction_context(std::shared_ptr<rocksdb::OptimisticTransactionDB> db);
    void begin(const bool use_snapshot = false);
    bool commit();
    std::shared_ptr<rocksdb::Transaction> txn() const;
private:
    std::shared_ptr<rocksdb::OptimisticTransactionDB> db_;
    std::shared_ptr<rocksdb::Transaction> txn_;
};

} // database
} // libbitcoin

#endif
