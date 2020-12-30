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
#ifndef LIBBITCOIN_DATABASE_TRANSACTION_DATABASE_HPP
#define LIBBITCOIN_DATABASE_TRANSACTION_DATABASE_HPP

#include <cstddef>
#include <boost/filesystem.hpp>
#include <bitcoin/system.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/result/transaction_result.hpp>
#include <bitcoin/database/unspent_outputs.hpp>
#include "rocksdb/db.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/optimistic_transaction_db.h"

namespace libbitcoin {
namespace database {

// Store transactions keyed by transaction hash.
// Block to transaction association is stored in block database.
class BCD_API transaction_database
{
public:
    /// Construct the database.
    transaction_database(rocksdb::OptimisticTransactionDB* db_,
        rocksdb::ColumnFamilyHandle* handle_,
        size_t cache_capacity);

    // Queries.
    //-------------------------------------------------------------------------

    /// Fetch transaction by its hash.
    transaction_result get(const system::hash_digest& hash) const;

    /// Populate tx metadata for the given block context.
    void get_block_metadata(const system::chain::transaction& tx,
        uint32_t forks, size_t fork_height) const;

    /// Populate tx metadata for the given transaction pool context.
    void get_pool_metadata(const system::chain::transaction& tx,
        uint32_t forks) const;

    /// Populate output metadata for the specified point and fork point.
    bool get_output(const system::chain::output_point& point,
        size_t fork_height) const;

    // Writers.
    // ------------------------------------------------------------------------

    /// Store a transaction not associated with a block.
    bool store(const system::chain::transaction& tx, uint32_t forks);

    /// Store a set of transactions (potentially from an unconfirmed block).
    bool store(const system::chain::transaction::list& transactions);

    /// Mark outputs spent by the candidate tx.
    bool candidate(const system::hash_digest& hash);

    /// Unmark outputs formerly spent by the candidate tx.
    bool uncandidate(const system::hash_digest& hash);

    /// Promote the transaction to confirmed (uncached).
    bool confirm(const system::hash_digest& hash, size_t height,
        uint32_t median_time_past, size_t position);

    /// Promote the set of transactions associated with a block to confirmed.
    bool confirm(const system::chain::block& block, size_t height,
        uint32_t median_time_past);

    /// Demote the set of transactions associated with a block to pooled.
    bool unconfirm(const system::chain::block& block);

private:
    typedef system::hash_digest key_type;

    // Store a transaction.
    //-------------------------------------------------------------------------
    bool storize(const system::chain::transaction& tx, size_t height,
        uint32_t median_time_past, size_t position);

    // Update the candidate state of the tx.
    //-------------------------------------------------------------------------
    bool candidate(const system::hash_digest& hash, bool positive);

    // Update the candidate spent of the output.
    bool candidate_spend(const system::chain::output_point& point,
        bool positive);

    // Update the candidate metadata of the existing tx.
    bool candidize(const system::hash_digest& hash, bool positive);

    // Update the spender height of the output.
    //-------------------------------------------------------------------------
    bool confirmed_spend(const system::chain::output_point& point,
        size_t spender_height);

    // Promote metadata of the existing tx to confirmed.
    bool confirmize(const system::hash_digest& hash, size_t height, uint32_t median_time_past,
        size_t position);

    std::shared_ptr<rocksdb::OptimisticTransactionDB> db_;
    rocksdb::ColumnFamilyHandle* handle_;

    // This is thread safe.
    unspent_outputs cache_;
};

} // namespace database
} // namespace libbitcoin

#endif
