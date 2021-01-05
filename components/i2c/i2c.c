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
    /* PCM3060 datasheet has no read operation, maybe it's not supported ? */
    ESP_LOGE(TAG, "Read is not implemented");
    return ESP_FAIL;
}
