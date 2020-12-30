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
#include <bitcoin/database/databases/transaction_database.hpp>

#include <cstddef>
#include <cstdint>
#include <boost/filesystem.hpp>
#include <bitcoin/system.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/result/transaction_result.hpp>
#include "rocksdb/db.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/optimistic_transaction_db.h"

namespace libbitcoin {
namespace database {

using namespace bc::system;
using namespace bc::system::chain;
using namespace bc::system::machine;

static constexpr auto no_time = 0u;

// Transactions uses a hash table index, O(1).
transaction_database::transaction_database(
    rocksdb::DB* db_,
    rocksdb::ColumnFamilyHandle* handle_,
    size_t cache_capacity)
  : db_(db_), handle_(handle_), cache_(cache_capacity)
{
}

// // Store.
// // ----------------------------------------------------------------------------

// // Store new unconfirmed tx and set tx
// bool transaction_database::store(const chain::transaction& tx, uint32_t forks)
// {
//     // Cache the unspent outputs of the unconfirmed transaction.
//     cache_.add(tx, forks, no_time, false);

//     return storize(tx, forks, no_time, transaction_result::unconfirmed);
// }

// // private
// bool transaction_database::storize(const chain::transaction& tx, size_t height,
//     uint32_t median_time_past, size_t position)
// {
//     BITCOIN_ASSERT(height <= max_uint32);
//     BITCOIN_ASSERT(position <= max_uint16);

//     // Assume the caller has not tested for existence (true for block update).
//     if (tx.metadata.link == transaction::validation::unlinked)
//     {
//         const auto element = hash_table_.find(tx.hash());

//         if (element)
//             tx.metadata.link = element.link();
//     }

//     // This allows address indexer to bypass indexing despite link existence.
//     tx.metadata.existed = tx.metadata.link != transaction::validation::unlinked;

//     // If the transaction already exists just return (with link set).
//     if (tx.metadata.existed)
//         return true;

//     const auto writer = [&](byte_serializer& serial)
//     {
//         serial.write_4_bytes_little_endian(static_cast<uint32_t>(height));
//         serial.write_2_bytes_little_endian(static_cast<uint16_t>(position));
//         serial.write_byte(transaction_result::candidate_false);
//         serial.write_4_bytes_little_endian(median_time_past);
//         tx.to_data(serial, false, true);
//     };

//     // Transactions are variable-sized.
//     const auto size = metadata_size + tx.serialized_size(false, true);

//     // Write the new transaction.
//     auto next = hash_table_.allocator();
//     tx.metadata.link = next.create(tx.hash(), writer, size);
//     hash_table_.link(next);
//     return true;
// }

} // namespace database
} // namespace libbitcoin
