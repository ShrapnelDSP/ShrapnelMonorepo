#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <span>

#include "juce_audio_basics/juce_audio_basics.h"
#include "main_thread.h"
#include "os/timer.h"
#include "timer_impl.h"

#define QUEUE_LEN 4

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

class FakeMidiUart final : public shrapnel::midi::MidiUartBase
{
public:
    std::optional<uint8_t> get_byte(TickType_t) override
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
                ((const char *key), (etl::string_view data)),
                (override));
    MOCK_METHOD(int,
                load,
                ((const char *key), (etl::istring & data)),
                (override));
};

class Integration : public ::testing::Test
{
protected:
    Integration()
        : audio_params(std::make_unique<shrapnel::AudioParameters>()),
          uut{send_message_fn, in_queue, &midi_uart, audio_params, storage}
    {
    }

    void pushMidiMessage(const juce::MidiMessage &message)
    {
        //const auto bytes = std::span<juce::uint8>(message.getRawData(), message.getRawDataSize());
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
    FakeStorage storage;
    shrapnel::MainThread<shrapnel::MAX_PARAMETERS, QUEUE_LEN> uut;
};

TEST_F(Integration, NotifiesServerAboutMidiMessages)
{
    using namespace shrapnel;

    auto note_on_message =
        juce::MidiMessage::noteOn(1, 0, static_cast<uint8_t>(1));
    pushMidiMessage(note_on_message);

    uut.loop();
    uut.loop();
    uut.loop();

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

    uut.loop();
}

TEST_F(Integration, MidiMappingCanBeCreated)
{
    using namespace shrapnel;

    // TODO: we are currently assuming these preconditions
    // Given default value is used for the ampGain parameter
    // And there are no midi mappings

    const auto mapping = std::pair<midi::Mapping::id_t, midi::Mapping>{
        {0},
        {
            .midi_channel = 1,
            .cc_number = 0,
            .mode = midi::Mapping::Mode::TOGGLE,
            .parameter_name{"ampGain"},
        },
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
                    midi::CreateResponse{mapping},
                    std::nullopt,
                }));

    uut.loop();
}

TEST_F(Integration, MidiCanUpdateParameters)
{
    using namespace shrapnel;

    const auto mapping = std::pair<midi::Mapping::id_t, midi::Mapping>{};

    // Given midi mapping has been set up
    pushServerApiMessage(AppMessage{
        midi::CreateRequest{
            {
                {0},
                {
                    .midi_channel = 1,
                    .cc_number = 0,
                    .mode = midi::Mapping::Mode::PARAMETER,
                    .parameter_name{"ampGain"},
                },
            },
        },
        std::nullopt,
    });

    uut.loop();

    // When the configured MIDI message is sent
    auto control_change_message = juce::MidiMessage::controllerEvent(1, 0, 0);
    pushMidiMessage(control_change_message);

    uut.loop();
    uut.loop();

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