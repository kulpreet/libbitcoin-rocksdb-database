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
#include "rocksdb/db.h"

namespace libbitcoin {
namespace database {

/// This class provides access to a rocksdb database
class data_base
{
public:
    typedef boost::filesystem::path path;
    typedef std::function<void(const system::code&)> result_handler;

    data_base(const path& directory, bool catalog, bool filter);

    // Open and close.
    // ------------------------------------------------------------------------

    /// Create and open all databases.
    bool create(const system::chain::block& genesis);

    /// Open all databases.
    bool open();

    /// Close all databases.
    bool close();

    /// Call close on destruct.
    ~data_base();

    /// Reader interfaces.
    // ------------------------------------------------------------------------
    // These are const to preclude write operations by public callers.

    // Node writers.
    // ------------------------------------------------------------------------

    // INITCHAIN (genesis)
    /// Push the block through candidacy and confirmation, without indexing.
    system::code push(const system::chain::block& block, size_t height=0,
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

private:
    // system::chain::transaction::list to_transactions(
    //     const block_result& result) const;

    // Path to db directory
    path directory_;
    rocksdb::DB* db_;

    const bool catalog_;
    const bool filter_;
};

} // namespace database
} // namespace libbitcoin

#endif
