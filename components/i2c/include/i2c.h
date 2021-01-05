#pragma once

#include <stdint.h>
#include <driver/i2c.h>

esp_err_t i2c_write_byte(i2c_port_t port, uint8_t slave_address, uint8_t
        reg_address, uint8_t data);

esp_err_t i2c_read_byte(i2c_port_t port, uint8_t slave_address, uint8_t
        reg_address, uint8_t *data);
