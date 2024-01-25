/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <memory>

#include "midi_mapping.h"
#include "midi_mapping_api.h"

namespace {

using namespace shrapnel;
using namespace shrapnel::midi;

using id_t = shrapnel::parameters::id_t;
using ::testing::FloatEq;
using ::testing::Not;
using ::testing::Return;

class MidiHandling : public ::testing::Test
{
protected:
    MidiHandling() : sut() {}

    MappingManager<2, 1> sut;
};

TEST_F(MidiHandling, Create)
{
    EXPECT_THAT(sut.get()->size(), 0);
    EXPECT_THAT(sut.create({{1}, {1, 2, Mapping::Mode::PARAMETER, "gain"}}), 0);
    EXPECT_THAT(sut.get()->size(), 1);
    EXPECT_THAT(sut.create({{2}, {3, 4, Mapping::Mode::PARAMETER, "volume"}}),
                0);
    EXPECT_THAT(sut.get()->size(), 2);
    EXPECT_THAT(sut.create({{2}, {5, 6, Mapping::Mode::PARAMETER, "tone"}}),
                Not(0));
}

TEST_F(MidiHandling, Update)
{
    EXPECT_EQ(0, sut.create({{1}, {1, 2, Mapping::Mode::PARAMETER, "gain"}}));

    // update non-existent should fail
    EXPECT_THAT(sut.update({{3}, {7, 8, Mapping::Mode::PARAMETER, "contour"}}),
                -1);
    EXPECT_THAT(sut.get()->size(), 1);

    EXPECT_EQ(0, sut.create({{2}, {3, 4, Mapping::Mode::PARAMETER, "volume"}}));
    EXPECT_THAT(sut.get()->size(), 2);

    EXPECT_THAT(sut.update({{1}, {5, 6, Mapping::Mode::PARAMETER, "tone"}}), 0);
    EXPECT_THAT(sut.get()->size(), 2);

    // TODO should we verify iteration here or something?
}

TEST_F(MidiHandling, Remove)
{
    EXPECT_EQ(0, sut.create({{1}, {1, 2, Mapping::Mode::PARAMETER, "gain"}}));
    sut.remove({1});
    EXPECT_THAT(sut.get()->size(), 0);
}

TEST_F(MidiHandling, Notifications)
{
    class Observer final : public midi::MappingObserver
    {
    public:
        void notification(const Mapping::id_t &) override
        {
            notification_count++;
        }

        int notification_count = 0;
    };

    Observer observer;
    sut.add_observer(observer);
    EXPECT_EQ(0, observer.notification_count);

    EXPECT_EQ(0, sut.create({{1}, {1, 2, Mapping::Mode::PARAMETER, "gain"}}));
    EXPECT_EQ(1, observer.notification_count);

    EXPECT_EQ(0, sut.update({{1}, {3, 4, Mapping::Mode::PARAMETER, "gain"}}));
    EXPECT_EQ(2, observer.notification_count);

    sut.remove({1});
    EXPECT_EQ(3, observer.notification_count);
}

TEST(MidiMappingPod, ToString)
{
    Mapping mapping{1, 2, Mapping::Mode::PARAMETER, parameters::id_t("test")};

    etl::string<128> buffer;
    etl::string_stream stream{buffer};
    stream << mapping;

    EXPECT_THAT(std::string(buffer.data()),
                "{ channel 1 cc number 2 mode parameter name optional with "
                "value test preset optional with no value }");
}

} // namespace
