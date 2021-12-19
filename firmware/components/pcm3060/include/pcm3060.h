#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_err.h>
#include <stdbool.h>
#include <driver/i2c.h>

/** \brief Initialise the driver */
void pcm3060_init(i2c_port_t port, bool adr);

/** \brief perform power up sequence */
esp_err_t pcm3060_power_up(void);

/** \brief perform power down sequence */
esp_err_t pcm3060_power_down(void);

#ifdef __cplusplus
}
#endif
