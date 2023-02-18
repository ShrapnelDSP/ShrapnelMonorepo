#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "main_thread.h"

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
          uut{mock_send_message.AsStdFunction(),
              in_queue,
              &midi_uart,
              audio_params,
              storage}
    {
    }

    testing::MockFunction<void(const AppMessage &)> mock_send_message;
    shrapnel::Queue<AppMessage, QUEUE_LEN> in_queue;
    FakeMidiUart midi_uart;
    std::shared_ptr<shrapnel::AudioParameters> audio_params;
    FakeStorage storage;
    shrapnel::MainThread<shrapnel::MAX_PARAMETERS, QUEUE_LEN> uut;
};

TEST_F(Integration, NotifiesServerAboutMidiMessages) {}
