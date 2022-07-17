#pragma once

#include <cstdint>
#include <driver/uart.h>
#include <driver/gpio.h>
#include <memory>

namespace shrapnel {
namespace midi {

class MidiUartBase {
    public:
    /** Block until a byte is received */
    virtual uint8_t get_byte(void) = 0;
};

class EspMidiUart final : public MidiUartBase {
    public:
    EspMidiUart(uart_port_t uart, gpio_num_t rx_io);

    uint8_t get_byte(void) override;

    private:
    uart_port_t uart;
};

}
}
