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
#include "mock_persistence.h"

namespace {

using namespace shrapnel;
using namespace shrapnel::midi;

using id_t = shrapnel::parameters::id_t;
using ::testing::FloatEq;
using ::testing::Not;
using ::testing::Return;

class MidiMapping : public ::testing::Test
{
    using SutType = MappingManager<2, 1>;

protected:
    MidiMapping() {}

    SutType create_sut() { return SutType{std::move(storage_mock)}; }

    std::unique_ptr<MockStorage> storage_mock = std::make_unique<MockStorage>();
};

TEST_F(MidiMapping, Create)
{
    auto sut = create_sut();
    EXPECT_THAT(sut.get()->size(), 0);
    uint32_t id = 0;
    EXPECT_THAT(sut.create({1, 2, Mapping::Mode::PARAMETER, "gain"}, id), 0);
    EXPECT_THAT(sut.get()->size(), 1);
    EXPECT_THAT(sut.create({3, 4, Mapping::Mode::PARAMETER, "volume"}, id), 0);
    EXPECT_THAT(sut.get()->size(), 2);
    EXPECT_THAT(sut.create({5, 6, Mapping::Mode::PARAMETER, "tone"}, id),
                Not(0));
}

TEST_F(MidiMapping, Update)
{
    uint32_t id;
    auto sut = create_sut();
    EXPECT_EQ(0, sut.create({1, 2, Mapping::Mode::PARAMETER, "gain"}, id));

    // update non-existent should fail
    EXPECT_THAT(sut.update(3, {7, 8, Mapping::Mode::PARAMETER, "contour"}), -1);
    EXPECT_THAT(sut.get()->size(), 1);

    EXPECT_EQ(0, sut.create({3, 4, Mapping::Mode::PARAMETER, "volume"}, id));
    EXPECT_THAT(sut.get()->size(), 2);

    EXPECT_THAT(sut.update(0, {5, 6, Mapping::Mode::PARAMETER, "tone"}), 0);
    EXPECT_THAT(sut.get()->size(), 2);

    // TODO should we verify iteration here or something?
}

TEST_F(MidiMapping, Destroy)
{
    uint32_t id;
    auto sut = create_sut();
    EXPECT_EQ(0, sut.create({1, 2, Mapping::Mode::PARAMETER, "gain"}, id));
    EXPECT_EQ(0, sut.destroy(id));
    EXPECT_THAT(sut.get()->size(), 0);
}

TEST_F(MidiMapping, Notifications)
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
    auto sut = create_sut();
    sut.add_observer(observer);
    EXPECT_EQ(0, observer.notification_count);

    uint32_t id;
    EXPECT_EQ(0, sut.create({1, 2, Mapping::Mode::PARAMETER, "gain"}, id));
    EXPECT_EQ(1, observer.notification_count);

    EXPECT_EQ(0, sut.update(id, {3, 4, Mapping::Mode::PARAMETER, "gain"}));
    EXPECT_EQ(2, observer.notification_count);

    EXPECT_EQ(0, sut.destroy(id));
    EXPECT_EQ(3, observer.notification_count);
}

TEST(MidiMappingPod, ToString)
{
    Mapping mapping{1, 2, Mapping::Mode::PARAMETER, parameters::id_t("test")};

    etl::string<128> buffer;
    etl::string_stream stream{buffer};
    stream << mapping;

    EXPECT_THAT(std::string(buffer.data()),
                "{ channel 1 cc number 2 mode parameter name test preset 0 }");
}

TEST(MidiMappingPod, GetRequestToString)
{
    MappingApiMessage message{GetRequest{}};

    etl::string<64> buffer;
    etl::string_stream stream{buffer};
    stream << message;

    EXPECT_THAT(std::string(buffer.data()), "<GetRequest>{}");
}

TEST(MidiMappingPod, CreateRequestToString)
{
    MappingApiMessage message{
        CreateRequest{
            .mapping{
                .midi_channel{1},
                .cc_number{2},
                .mode{Mapping::Mode::TOGGLE},
                .parameter_name{"test"},
            },
        },
    };

    etl::string<128> buffer;
    etl::string_stream stream{buffer};
    stream << message;

    EXPECT_THAT(std::string(buffer.data()),
                "<CreateRequest>{ { channel 1 cc number 2 mode toggle name "
                "test preset 0 } }");
}

TEST(MidiMappingPod, CreateResponseToString)
{
    MappingApiMessage message{
        CreateResponse{
            {
                42,
                {
                    .midi_channel{1},
                    .cc_number{2},
                    .mode{Mapping::Mode::TOGGLE},
                    .parameter_name{"test"},
                },
            },
        },
    };

    etl::string<128> buffer;
    etl::string_stream stream{buffer};
    stream << message;

    EXPECT_THAT(std::string(buffer.data()),
                "<CreateResponse>{ { 42, { channel 1 cc number 2 mode toggle "
                "name test preset 0 } } }");
}

TEST(MidiMappingPod, UpdateToString)
{
    MappingApiMessage message{
        Update{
            {
                42,
                {
                    .midi_channel{1},
                    .cc_number{2},
                    .mode{Mapping::Mode::TOGGLE},
                    .parameter_name{"test"},
                },
            },
        },
    };

    etl::string<128> buffer;
    etl::string_stream stream{buffer};
    stream << message;

    EXPECT_THAT(std::string(buffer.data()),
                "<Update>{ { 42, { channel 1 cc number 2 mode toggle name test "
                "preset 0 } } }");
} // namespace

TEST(MidiMappingPod, RemoveToString)
{
    MappingApiMessage message{
        Remove{42},
    };

    etl::string<128> buffer;
    etl::string_stream stream{buffer};
    stream << message;

    EXPECT_THAT(std::string(buffer.data()), "<Remove>{ 42 }");
}

} // namespace
