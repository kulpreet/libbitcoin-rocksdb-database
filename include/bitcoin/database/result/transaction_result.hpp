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
#ifndef LIBBITCOIN_ROCKSDB_DATABASE_TRANSACTION_RESULT_HPP
#define LIBBITCOIN_ROCKSDB_DATABASE_TRANSACTION_RESULT_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/system.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/result/inpoint_iterator.hpp>

namespace libbitcoin {
namespace database {

/// The class uses rocksdb iterator to provide libbitcoin database
/// iterator interface.
class BCD_API transaction_result
{
public:
    /// This is the store value for candidate true.
    static const uint8_t candidate_true;

    /// This is the store value for candidate false.
    static const uint8_t candidate_false;

    /// This is unconfirmed tx height (forks) sentinel.
    static const uint32_t unverified;

    /// This is unconfirmed tx position sentinel.
    static const uint16_t unconfirmed;

    /// This is deconfirmed tx position sentinel.
    static const uint16_t deconfirmed;

    transaction_result(system::shared_mutex& metadata_mutex);

    /// True if this transaction result is valid (found).
    operator bool() const;

    /// The transaction hash (from cache).
    system::hash_digest hash() const;

    /// The height of the block of the tx, or forks if unconfirmed or deconfirmed.
    size_t height() const;

    /// The ordinal position of the tx in a block, or unconfirmed or deconfirmed.
    size_t position() const;

    /// The transaction is in a candidate block.
    bool candidate() const;

    /// The median time past of the block which includes the transaction.
    uint32_t median_time_past() const;

    /// All tx outputs confirmed below fork or as candidates.
    bool is_candidate_spent(size_t fork_height) const;

    /////// All tx outputs confirmed below fork.
    ////bool is_confirmed_spent(size_t fork_height) const;

    /// The output at the specified index within this transaction.
    system::chain::output output(uint32_t index) const;

    /// The transaction, optionally including witness.
    system::chain::transaction transaction(bool witness=true) const;

    /// Iterate over the input set.
    inpoint_iterator begin() const;
    inpoint_iterator end() const;

private:
    bool candidate_;
    uint32_t height_;
    uint16_t position_;
    uint32_t median_time_past_;


    // Metadata values are kept consistent by mutex.
    system::shared_mutex& metadata_mutex_;
};

} // namespace database
} // namespace libbitcoin

#endif
