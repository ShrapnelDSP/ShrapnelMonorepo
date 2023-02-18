#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <span>

#include "main_thread.h"
#include "os/timer.h"
#include "timer_impl.h"

#define QUEUE_LEN 4

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

    void pushMidiBytes(std::vector<uint8_t> bytes)
    {
        std::for_each(bytes.begin(),
                      bytes.end(),
                      [this](uint8_t byte) { midi_uart.buffer.push(byte); });
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
    using namespace shrapnel::midi;

    pushMidiBytes({
        0x90,
        0x00,
        0x01,
    });

    uut.loop();
    uut.loop();
    uut.loop();

    EXPECT_CALL(send_message,
                Call(AppMessage{
                    MessageReceived{
                        {
                            Message{
                                .channel{1},
                                .parameters{
                                    Message::NoteOn{.note{0}, .velocity{1}},
                                },
                            },
                        },
                    },
                    std::nullopt,
                }));

    shrapnel::os::Timer::impl::tick(pdMS_TO_TICKS(200));

    uut.loop();
}
