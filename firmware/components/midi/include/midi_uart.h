#pragma once

#include <cstdint>
#include <driver/uart.h>
#include <driver/gpio.h>
#include <memory>
#include "queue.h"

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

    // TODO change this to a constructor output parameter of type QueueHandle_t?
    //
    // This is so messy, and it is only going to get used by the main thread
    // for debugging events.
    //
    // The root cause of the problem is that this is an output parameter of
    // espressif's uart initialisation function.
    //
    // Queue<uart_event_t> queue;
    private:
    uart_port_t uart;
};

}
}
