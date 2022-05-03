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

#include "i2c.h"

#include "esp_log.h"
#define TAG "i2c"

esp_err_t i2c_write_byte(i2c_port_t port, uint8_t slave_address, uint8_t reg_address, uint8_t data)
{
    esp_err_t err = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    if(cmd == NULL)
    {
        goto cleanup;
        err = ESP_ERR_NO_MEM;
    }

    err = i2c_master_start(cmd);
    if(err != ESP_OK) goto cleanup;

    err = i2c_master_write_byte(cmd, slave_address, true);
    if(err != ESP_OK) goto cleanup;

    err = i2c_master_write_byte(cmd, reg_address, true);
    if(err != ESP_OK) goto cleanup;

    err = i2c_master_write_byte(cmd, data, true);
    if(err != ESP_OK) goto cleanup;

    err = i2c_master_stop(cmd);
    if(err != ESP_OK) goto cleanup;

    err = i2c_master_cmd_begin(port, cmd, 100 / portTICK_PERIOD_MS);
    if(err != ESP_OK) goto cleanup;

cleanup:
    if(cmd)
    {
        i2c_cmd_link_delete(cmd);
    }
    return err;
}

esp_err_t i2c_read_byte(i2c_port_t port, uint8_t slave_address, uint8_t reg_address, uint8_t *data)
{
    (void) port;
    (void) slave_address;
    (void) reg_address;
    (void) data;

    /* PCM3060 datasheet has no read operation, maybe it's not supported ? */
    ESP_LOGE(TAG, "Read is not implemented");
    return ESP_FAIL;
}
