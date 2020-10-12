#include "driver/i2s.h"
#include "i2s.h"

#define BITS            (24)
#define MCK             (384*SAMPLE_RATE)

#define SAMPLE_RATE     (48000)
#define I2S_NUM         (1)
#define I2S_BCK_IO      (GPIO_NUM_2)
#define I2S_WS_IO       (GPIO_NUM_4)
#define I2S_DO_IO       (GPIO_NUM_5)
#define I2S_DI_IO       (-1)

static void fill_dma_buffer()
{
    //XXX not sure how many bytes there are exactly, but this fills up the
    //whole buffer for a continous output
    const int buffer_size_bytes = 6*60*4*4;
    int *samples_data = malloc(buffer_size_bytes);
    size_t i2s_bytes_write = 0;

    for(int i = 0; i < buffer_size_bytes/4/2; i++) {
        samples_data[i*2] = ((int) i<<8);
        samples_data[i*2 + 1] = ((int) i<<8);
    }

    i2s_set_clk(I2S_NUM, SAMPLE_RATE, BITS, 2);
    i2s_write(I2S_NUM, samples_data, buffer_size_bytes, &i2s_bytes_write, 100);
    printf("wrote %zu bytes out of %d\n", i2s_bytes_write, buffer_size_bytes);

    free(samples_data);
}

/**
 * Set up I2S
 * 
 * Master mode, MCK, BCK, FS controlled using the defines above. Clock is
 * output on GPIO0 (CLK_OUT1) to keep UART0 free.
 *
 * TODO Make connect this to DMA so that we can receive and send data to the
 * codec. Maybe we need to use both I2S for full duplex?
 */
void i2s_setup(void)
{
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .fixed_mclk = MCK,
        .bits_per_sample = BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
        .use_apll = true,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = I2S_DI_IO
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);

    /* Route the I2S master clock to CLK_OUT1/GPIO0 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
#if I2S_NUM == 0
    WRITE_PERI_REG(PIN_CTRL, READ_PERI_REG(PIN_CTRL) & 0xFFFFFFF0);
#else
    WRITE_PERI_REG(PIN_CTRL, READ_PERI_REG(PIN_CTRL) | 0x0000000F);
#endif

    fill_dma_buffer();
}
