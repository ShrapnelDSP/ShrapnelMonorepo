#include "pcm3060.h"
#include <stdint.h>
#include "i2c.h"
#include <esp_log.h>

#define TAG "pcm3060"
#define I2C_ADDRESS 0x8C

static uint8_t _address = -1;
static i2c_port_t _port = -1;

static bool _initialised(void)
{
    return _port != -1;
}


esp_err_t _i2c_write(uint8_t reg_adr, uint8_t data)
{
    esp_err_t err = i2c_write_byte(_port, _address, reg_adr, data);

    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Write to reg 0x%02x failed", reg_adr);
    }

    return err;
}

void pcm3060_init(i2c_port_t port, bool adr)
{
    _port = port;
    _address = I2C_ADDRESS | (adr << 1);
}

esp_err_t pcm3060_power_up(void)
{
    esp_err_t err;

    if(!_initialised())
    {
        return ESP_FAIL;
    }

    err = _i2c_write(0x43, 0x80);//use clock1 for DAC, slave mode, 24-bit I2S
    if(err != ESP_OK)
    {
        return err;
    }

    err = _i2c_write(0x40, 0xC0);//differential output, power save release
    return err;
}

esp_err_t pcm3060_power_down(void)
{
    if(!_initialised())
    {
        return ESP_FAIL;
    }

    return _i2c_write(0x40, 0xF1);//single ended output, power save enable
}
