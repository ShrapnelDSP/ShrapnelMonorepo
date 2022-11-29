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

#pragma once

#include <cstdint>
#include <driver/uart.h>
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include <memory>
#include <optional>

namespace shrapnel {
namespace midi {

class MidiUartBase {
    public:
    virtual std::optional<uint8_t> get_byte(TickType_t timeout) = 0;
};

class EspMidiUart final : public MidiUartBase {
    public:
    EspMidiUart(uart_port_t uart, gpio_num_t rx_io);

    std::optional<uint8_t> get_byte(TickType_t timeout) override;

    private:
    uart_port_t uart;
};

}
}
