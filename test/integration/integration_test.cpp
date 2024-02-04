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
#include <mock_persistence.h>
#include <span>

#include "juce_audio_basics/juce_audio_basics.h"
#include "main_thread.h"
#include "os/timer.h"
#include "timer_impl.h"

#define QUEUE_LEN 4

extern "C" {

__attribute__((__noreturn__)) void
_esp_error_check_failed(esp_err_t rc,
                        const char *file,
                        int line,
                        const char *function,
                        const char *expression)

{
    assert(false);
}

void _esp_error_check_failed_without_abort(esp_err_t rc,
                                           const char *file,
                                           int line,
                                           const char *function,
                                           const char *expression){};
}

namespace shrapnel::midi {

template <typename T>
    requires(!std::is_pointer_v<T>)
std::ostream &operator<<(std::ostream &out, const T &message)
{
    etl::string<128> buffer;
    etl::string_stream stream{buffer};
    stream << message;

    return out << buffer.data();
}

} // namespace shrapnel::midi

class FakeMidiUart final : public shrapnel::midi::MidiUartBase<int>
{
public:
    std::optional<uint8_t> get_byte(int) override
    {
        if(buffer.empty())
            return std::nullopt;

        auto out = buffer.front();
        buffer.pop();
        return out;
    }

    std::queue<uint8_t> buffer;
};

class FakeStorage final : public shrapnel::persistence::Storage
{
public:
    MOCK_METHOD(int,
                save,
                ((const char *key), (std::span<uint8_t> data)),
                (override));
    MOCK_METHOD(int,
                save,
                ((const char *key), (etl::string_view data)),
                (override));
    MOCK_METHOD(int, save, ((const char *key), (uint32_t data)), (override));
    MOCK_METHOD(int, save, ((const char *key), (float data)), (override));
    MOCK_METHOD(int,
                load,
                ((const char *key), (std::span<uint8_t> & data)),
                (override));
    MOCK_METHOD(int,
                load,
                ((const char *key), (etl::istring & data)),
                (override));
    MOCK_METHOD(int, load, ((const char *key), (uint32_t & data)), (override));
    MOCK_METHOD(int, load, ((const char *key), (float &data)), (override));
};

class Integration : public ::testing::Test
{
    using UutType = shrapnel::MainThread<shrapnel::MAX_PARAMETERS, QUEUE_LEN>;

protected:
    Integration()
        : audio_params(std::make_unique<shrapnel::AudioParameters>()),
          storage{std::make_shared<FakeStorage>()},
          midi_mapping_storage{std::make_unique<shrapnel::MockStorage>()},
          presets_storage{std::make_unique<shrapnel::MockStorage>()}
    {
    }

    UutType create_uut()
    {
        return {
            send_message_fn,
            in_queue,
            audio_params,
            storage,
            std::move(midi_mapping_storage),
            std::move(presets_storage),
        };
    };

    void pushMidiMessage(const juce::MidiMessage &message)
    {
        std::for_each(message.getRawData(),
                      message.getRawData() + message.getRawDataSize(),
                      [this](uint8_t byte) { midi_uart.buffer.push(byte); });
    }

    void pushServerApiMessage(const AppMessage &message)
    {
        auto rc = in_queue.send(&message, 0);
        ASSERT_THAT(rc, pdPASS);
    }

    testing::MockFunction<void(const AppMessage &)> send_message;
    std::function<void(const AppMessage &)> send_message_fn =
        send_message.AsStdFunction();
    shrapnel::Queue<AppMessage, QUEUE_LEN> in_queue;
    FakeMidiUart midi_uart;
    std::shared_ptr<shrapnel::AudioParameters> audio_params;
    std::shared_ptr<FakeStorage> storage;
    std::unique_ptr<shrapnel::MockStorage> midi_mapping_storage;
    std::unique_ptr<shrapnel::MockStorage> presets_storage;
};

TEST_F(Integration, NotifiesServerAboutMidiMessages)
{
    using namespace shrapnel;

    auto note_on_message =
        juce::MidiMessage::noteOn(1, 0, static_cast<uint8_t>(1));
    pushMidiMessage(note_on_message);

    shrapnel::os::Timer::impl::tick(pdMS_TO_TICKS(200));

    EXPECT_CALL(
        send_message,
        Call(AppMessage{
            midi::MessageReceived{
                {
                    midi::Message{
                        .channel = 1,
                        .parameters{
                            midi::Message::NoteOn{.note = 0, .velocity = 1},
                        },
                    },
                },
            },
            std::nullopt,
        }));

    auto uut = create_uut();
    uut.loop();
}

TEST_F(Integration, MidiMappingCanBeCreated)
{
    using namespace shrapnel;

    const auto mapping = midi::Mapping{
        .midi_channel = 1,
        .cc_number = 0,
        .mode = midi::Mapping::Mode::TOGGLE,
        .parameter_name{"ampGain"},
    };

    const auto create_mapping_request = AppMessage{
        midi::CreateRequest{mapping},
        std::nullopt,
    };

    // When a new mapping is created
    pushServerApiMessage(create_mapping_request);

    // Then appropriate response is sent to the API
    EXPECT_CALL(send_message,
                Call(AppMessage{
                    midi::CreateResponse{{0, mapping}},
                    std::nullopt,
                }));

    auto uut = create_uut();
    uut.loop();
}

TEST_F(Integration, MidiCanUpdateParameters)
{
    using namespace shrapnel;

    // Given midi mapping has been set up
    pushServerApiMessage(AppMessage{
        midi::CreateRequest{
            {
                .midi_channel = 1,
                .cc_number = 0,
                .mode = midi::Mapping::Mode::PARAMETER,
                .parameter_name{"ampGain"},
            },
        },
        std::nullopt,
    });

    auto uut = create_uut();
    uut.loop();

    // When the configured MIDI message is sent
    auto control_change_message = juce::MidiMessage::controllerEvent(1, 0, 0);
    pushMidiMessage(control_change_message);

    // Then the configured parameter changes
    EXPECT_CALL(send_message,
                Call(AppMessage{
                    parameters::Update{
                        .id{"ampGain"},
                        .value = 0.f,
                    },
                    std::nullopt,
                }));

    uut.loop();

    // And the change affects the audio output
    // TODO
}