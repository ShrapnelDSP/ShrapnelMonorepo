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

    ESP_ERROR_CHECK(uart_param_config(uart, &config));

    ESP_ERROR_CHECK(uart_driver_install(
                uart,
                UART_FIFO_LEN + 1,
                0,
                0,
                NULL,
                0));
}

std::optional<uint8_t> EspMidiUart::get_byte(TickType_t timeout) {
    uint8_t out;
    int rc = uart_read_bytes(uart, &out, 1, timeout);
    assert((rc == 0) || (rc == 1));

    if(rc == 0)
    {
        return std::nullopt;
    }

    return out;
}

}
}
