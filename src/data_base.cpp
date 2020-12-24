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
  : directory_(directory), catalog_(catalog), filter_(filter)
{
}

data_base::~data_base()
{
    close();
}

bool
data_base::create(const system::chain::block& genesis)
{
    return open();
}

bool
data_base::open()
{
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, directory_.string(), &db_);
    return status.ok();
}

bool
data_base::close()
{
    rocksdb::Status status = db_->Close();
    return status.ok();
}

} // namespace database
} // namespace libbitcoin
