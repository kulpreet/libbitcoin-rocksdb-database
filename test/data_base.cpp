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
#include <boost/test/unit_test.hpp>

#include <boost/filesystem.hpp>
#include <bitcoin/database.hpp>
#include "./utility/utility.hpp"

using namespace boost::system;
using namespace boost::filesystem;
using namespace bc;
using namespace bc::database;
using namespace bc::system;
using namespace bc::system::chain;

#define DIRECTORY "data_base"
#define TRANSACTION1 "0100000001537c9d05b5f7d67b09e5108e3bd5e466909cc9403ddd98bc42973f366fe729410600000000ffffffff0163000000000000001976a914fe06e7b4c88a719e92373de489c08244aee4520b88ac00000000"
#define TRANSACTION2 "010000000147811c3fc0c0e750af5d0ea7343b16ea2d0c291c002e3db778669216eb689de80000000000ffffffff0118ddf505000000001976a914575c2f0ea88fcbad2389a372d942dea95addc25b88ac00000000"

static BC_CONSTEXPR auto file_path = DIRECTORY "/tx_database";

struct data_base_directory_setup_fixture
{
    data_base_directory_setup_fixture()
    {
        test::clear_path(DIRECTORY);
    }

    ~data_base_directory_setup_fixture()
    {
        test::clear_path(DIRECTORY);
    }
};

BOOST_FIXTURE_TEST_SUITE(data_base_tests, data_base_directory_setup_fixture)

BOOST_AUTO_TEST_CASE(data_base__create__instance__success)
{
    std::cerr << file_path << std::endl;
    data_base instance(file_path, false, false);

    const auto bc_settings = bc::system::settings(config::settings::mainnet);
    BOOST_CHECK(instance.create(bc_settings.genesis_block));
    BOOST_CHECK(instance.close());

    BOOST_CHECK(instance.open());
    BOOST_CHECK(instance.close());
}

BOOST_AUTO_TEST_SUITE_END()
