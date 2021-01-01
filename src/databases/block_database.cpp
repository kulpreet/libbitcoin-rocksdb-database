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
#include <bitcoin/database/databases/block_database.hpp>

#include <cstddef>
#include <cstdint>
#include <boost/filesystem.hpp>
#include <bitcoin/system.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/result/block_result.hpp>
#include "rocksdb/db.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/optimistic_transaction_db.h"

namespace libbitcoin {
namespace database {

using namespace bc::system;
using namespace bc::system::chain;
using namespace bc::system::machine;

static const auto header_size = header::satoshi_fixed_size();
static constexpr auto median_time_past_size = sizeof(uint32_t);
static constexpr auto height_size = sizeof(uint32_t);
static constexpr auto state_size = sizeof(uint8_t);
static constexpr auto checksum_size = sizeof(uint32_t);

static const auto height_offset = header_size + median_time_past_size;
static const auto state_offset = height_offset + height_size;
static const auto checksum_offset = state_offset + state_size;

// Total size of block header and metadata storage without neutrino filter offset.
static const auto base_block_size = header_size + median_time_past_size +
    height_size + state_size + checksum_size;

block_database::block_database(std::shared_ptr<rocksdb::OptimisticTransactionDB> db_,
    rocksdb::ColumnFamilyHandle* block_handle_,
    rocksdb::ColumnFamilyHandle* block_transactions_handle_)
  : db_(db_), block_handle_(block_handle_), block_transactions_handle_(block_transactions_handle_)
{
}

// block_result
// block_database::get(std::shared_ptr<transaction_context> context,
//         const system::hash_digest& hash) const
// {
//     std::string value;
//     context->txn()->Get(rocksdb::ReadOptions(), handle_, hash, &value);
// }


void block_database::store(std::shared_ptr<transaction_context> context,
    const system::chain::header& header, size_t height,
    uint32_t median_time_past)
{
    static constexpr auto no_checksum = 0u;

    // New headers are only accepted in the candidate state.
    store(context, header, height, median_time_past, no_checksum, block_state::candidate);
}

void block_database::store(std::shared_ptr<transaction_context> context,
    const system::chain::header& header, size_t height,
    uint32_t median_time_past, uint32_t checksum, uint8_t state)
{
    BITCOIN_ASSERT(height <= max_uint32);
    BITCOIN_ASSERT(!header.metadata.exists);

    const auto writer = [&](byte_serializer& serial)
    {
        header.to_data(serial, false);
        serial.write_4_bytes_little_endian(median_time_past);
        serial.write_4_bytes_little_endian(static_cast<uint32_t>(height));
        serial.write_byte(state);
        serial.write_4_bytes_little_endian(checksum);
    };
    void* value = malloc(base_block_size);
    //context->txn()->Put(block_handle_, header.hash(), value);
}

} // namespace database
} // namespace libbitcoin
