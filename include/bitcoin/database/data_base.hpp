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
#ifndef LIBBITCOIN_ROCKSDB_DATABASE_DATA_BASE_HPP
#define LIBBITCOIN_ROCKSDB_DATABASE_DATA_BASE_HPP

#include <cstddef>
#include <functional>
#include <bitcoin/system.hpp>
#include <bitcoin/database/transaction_context.hpp>
#include <bitcoin/database/databases/block_database.hpp>
#include <bitcoin/database/databases/transaction_database.hpp>
#include "rocksdb/db.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/optimistic_transaction_db.h"

namespace libbitcoin {
namespace database {

/// This class provides access to a rocksdb database
class data_base
{
public:
    const std::string TRANSACTIONS_COLUMN_FAMILY = "transactions";
    const std::string BLOCKS_COLUMN_FAMILY = "blocks";
    const size_t CACHE_CAPACITY = 10000;
    typedef boost::filesystem::path path;
    typedef std::function<void(const system::code&)> result_handler;

    data_base(const path& directory, bool catalog, bool filter);

    // Open and close.
    // ------------------------------------------------------------------------

    /// Create and open all databases.
    bool create(const system::chain::block& genesis);

    /// Open existing rocksdb database. Returns false if it doesn't exist.
    bool open();

    /// Close all databases.
    bool close();

    /// Call close on destruct.
    ~data_base();

    /// Database transaction interface
    // ------------------------------------------------------------------------
    std::shared_ptr<transaction_context> begin_transaction(
        bool use_snapshot = false);
    bool commit_transaction(std::shared_ptr<transaction_context> txn);

    /// Reader interfaces.
    // ------------------------------------------------------------------------
    // These are const to preclude write operations by public callers.

    // Node writers.
    // ------------------------------------------------------------------------

    // INITCHAIN (genesis)
    /// Push the block through candidacy and confirmation, without indexing.
    system::code push(std::shared_ptr<transaction_context> context,
        const system::chain::block& block, size_t height=0,
        uint32_t median_time_past=0);

    // HEADER ORGANIZER (reorganize)
    /// Reorganize the header index to the specified fork point.
    system::code reorganize(const system::config::checkpoint& fork_point,
        system::header_const_ptr_list_const_ptr incoming,
        system::header_const_ptr_list_ptr outgoing);

    // BLOCK ORGANIZER (update)
    /// Update the stored block with txs.
    system::code update(const system::chain::block& block, size_t height);

    // BLOCK ORGANIZER (update, invalidate)
    /// Set header validation state and metadata.
    system::code invalidate(const system::chain::header& header,
        const system::code& error);

    // BLOCK ORGANIZER (candidate)
    /// Mark candidate block, txs and outputs spent by them as candidate.
    system::code candidate(const system::chain::block& block);

    // BLOCK ORGANIZER (reorganize)
    /// Reorganize the block index to the specified fork point.
    system::code reorganize(const system::config::checkpoint& fork_point,
        system::block_const_ptr_list_const_ptr incoming,
        system::block_const_ptr_list_ptr outgoing);

    // BLOCK ORGANIZER (confirm)
    /// Confirm candidate block with confirmed parent.
    system::code confirm(const system::hash_digest& block_hash,
        size_t height);

    // TRANSACTION ORGANIZER (store)
    /// Store unconfirmed tx/payments that were verified with the given forks.
    system::code store(const system::chain::transaction& tx, uint32_t forks);

    // TRANSACTION ORGANIZER (store)
    /// Add transaction payment to the payment index.
    system::code catalog(const system::chain::transaction& tx);

    // Databases.
    // ------------------------------------------------------------------------

    std::shared_ptr<block_database> blocks_;
    std::shared_ptr<transaction_database> transactions_;

    /// Reader interfaces.
    // ------------------------------------------------------------------------
    // These are const to preclude write operations by public callers.

    const block_database& blocks() const;

    const transaction_database& transactions() const;

private:
    // system::chain::transaction::list to_transactions(
    //     const block_result& result) const;

    // Path to db directory
    path directory_;
    rocksdb::OptimisticTransactionDB* dbp_;
    std::shared_ptr<rocksdb::OptimisticTransactionDB> db_;

    std::atomic<bool> closed_;

    const bool catalog_;
    const bool filter_;

    // rocksdb column families for all databases
    std::vector<rocksdb::ColumnFamilyHandle*> column_family_handles_;
};

} // namespace database
} // namespace libbitcoin

#endif
