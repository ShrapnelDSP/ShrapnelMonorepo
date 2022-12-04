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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <memory>

#include "midi_mapping.h"
#include "midi_mapping_api.h"

namespace {

using namespace shrapnel;
using namespace shrapnel::midi;

using id_t = shrapnel::parameters::id_t;
using ::testing::Not;

class MockAudioParameters
{
    public:
    MOCK_METHOD(int, update, (id_t param, float value), ());
};

class MidiMapping : public ::testing::Test
{
    protected:

    MidiMapping() : sut(parameters_mock) {}

    std::shared_ptr<MockAudioParameters> parameters_mock = std::make_shared<MockAudioParameters>();
    MappingManager<MockAudioParameters, 2, 1> sut;
};

TEST_F(MidiMapping, Create)
{
    EXPECT_THAT(sut.get()->size(), 0);
    EXPECT_THAT(sut.create({{1}, {1, 2, "gain"}}), 0);
    EXPECT_THAT(sut.get()->size(), 1);
    EXPECT_THAT(sut.create({{2}, {3, 4, "volume"}}), 0);
    EXPECT_THAT(sut.get()->size(), 2);
    EXPECT_THAT(sut.create({{2}, {5, 6, "tone"}}), Not(0));
}

TEST_F(MidiMapping, Process)
{
    sut.create({{1}, {1, 2, "gain"}});

    EXPECT_CALL(*parameters_mock, update(id_t("gain"), 0.f));
    sut.process_message({
        .channel{1},
        .parameters{
            Message::ControlChange{.control = 2, .value = 0}
        },
    });

    EXPECT_CALL(*parameters_mock, update(id_t("gain"), 1.f));
    sut.process_message({
        .channel{1},
        .parameters{
            Message::ControlChange{.control = 2, .value = CC_VALUE_MAX}
        },
    });

    EXPECT_CALL(*parameters_mock, update).Times(0);
    sut.process_message({
        .channel{99},
        .parameters{
            Message::ControlChange{.control = 2, .value = 0}
        },
    });

    sut.process_message({
        .channel{1},
        .parameters{
            Message::ControlChange{.control = 99, .value = 0}
        },
    });
}

TEST_F(MidiMapping, Update)
{
    sut.create({{1}, {1, 2, "gain"}});

    // update non-existent should fail
    EXPECT_THAT(sut.update({{3}, {7, 8, "contour"}}), -1);
    EXPECT_THAT(sut.get()->size(), 1);

    sut.create({{2}, {3, 4, "volume"}});
    EXPECT_THAT(sut.get()->size(), 2);

    EXPECT_THAT(sut.update({{1}, {5, 6, "tone"}}), 0);
    EXPECT_THAT(sut.get()->size(), 2);

    EXPECT_CALL(*parameters_mock, update).Times(0);
    sut.process_message({
        .channel{1},
        .parameters{
            Message::ControlChange{.control = 2, .value = 0}
        },
    });

    EXPECT_CALL(*parameters_mock, update(id_t("tone"), 0));
    sut.process_message({
        .channel{5},
        .parameters{
            Message::ControlChange{.control = 6, .value = 0}
        },
    });
}

TEST_F(MidiMapping, Remove)
{
    sut.create({{1}, {1, 2, "gain"}});
    sut.remove({1});
    EXPECT_THAT(sut.get()->size(), 0);
}

TEST_F(MidiMapping, Notifications)
{
    class Observer final : public midi::MappingObserver {
    public:
        void notification(const Mapping::id_t &) override {
            notification_count++;
        }

        int notification_count = 0;
    };


    Observer observer;
    sut.add_observer(observer);
    EXPECT_EQ(0, observer.notification_count);

    sut.create({{1}, {1, 2, "gain"}});
    EXPECT_EQ(1, observer.notification_count);

    sut.update({{1}, {3, 4, "gain"}});
    EXPECT_EQ(2, observer.notification_count);

    sut.remove({1});
    EXPECT_EQ(3, observer.notification_count);
}

TEST(MidiMappingPod, ToString)
{
    Mapping mapping{1, 2, parameters::id_t("test")};

    etl::string<64> buffer;
    etl::string_stream stream{buffer};
    stream << mapping;

    EXPECT_THAT(std::string(buffer.data()), "{ channel 1 cc number 2 name test }");
}

}
