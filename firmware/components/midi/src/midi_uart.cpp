#include "midi_uart.h"
#include <assert.h>

namespace shrapnel {
namespace midi {

EspMidiUart::EspMidiUart(uart_port_t a_uart, gpio_num_t rx_io) : uart(a_uart) {
    uart_config_t config = {
        .baud_rate = 31250,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_set_pin(uart, UART_PIN_NO_CHANGE, rx_io, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    QueueHandle_t uart_queue = nullptr;

    ESP_ERROR_CHECK(uart_param_config(uart, &config));

    ESP_ERROR_CHECK(uart_driver_install(
                uart,
                UART_FIFO_LEN + 1,
                0,
                10,
                &uart_queue,
                0));

    assert(uart_queue != nullptr);
}

uint8_t EspMidiUart::get_byte(void) {
    uint8_t out = 0;
    int rc = uart_read_bytes(uart, &out, 1, portMAX_DELAY);
    assert(rc == 1);
    return out;
}

}
}
