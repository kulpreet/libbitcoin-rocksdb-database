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
#ifndef LIBBITCOIN_DATABASE_BLOCK_DATABASE_HPP
#define LIBBITCOIN_DATABASE_BLOCK_DATABASE_HPP

#include <atomic>
#include <cstddef>
#include <boost/filesystem.hpp>
#include <bitcoin/system.hpp>
#include <bitcoin/database/transaction_context.hpp>
#include <bitcoin/database/block_state.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/result/block_result.hpp>
#include "rocksdb/db.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/optimistic_transaction_db.h"

namespace libbitcoin {
namespace database {

/// Stores block_headers each with a list of transaction indexes.
/// Lookup possible by hash or height.
class BCD_API block_database
{
public:
    /// Construct the database.
    block_database(std::shared_ptr<rocksdb::OptimisticTransactionDB> db_,
        rocksdb::ColumnFamilyHandle* handle_);

    // Queries.
    //-------------------------------------------------------------------------

    /// The height of the highest candidate|confirmed block.
    bool top(size_t& out_height, bool candidate) const;

    /// Fetch block by block|header index height.
    block_result get(size_t height, bool candidate) const;

    /// Fetch block by hash.
    block_result get(std::shared_ptr<transaction_context> context,
        const system::hash_digest& hash) const;

    /// Populate header metadata for the given header.
    void get_header_metadata(const system::chain::header& header) const;

    // Writers.
    // ------------------------------------------------------------------------

    /// Store header, validated at height, candidate, pending (but unindexed).
    void store(const system::chain::header& header, size_t height,
        uint32_t median_time_past);

    /// Populate pooled block transaction references, state is unchanged.
    bool update_transactions(const system::chain::block& block);

    /// Promote pooled block to valid|invalid and set code.
    bool validate(const system::hash_digest& hash, const system::code& error);

    /// Promote pooled|candidate block to candidate|confirmed respectively.
    bool promote(const system::hash_digest& hash, size_t height, bool candidate);

    /// Demote candidate|confirmed header to pooled|pooled (not candidate).
    bool demote(const system::hash_digest& hash, size_t height,
        bool candidate);

private:
    std::shared_ptr<rocksdb::OptimisticTransactionDB> db_;
    rocksdb::ColumnFamilyHandle* handle_;
};

} // namespace database
} // namespace libbitcoin

#endif
