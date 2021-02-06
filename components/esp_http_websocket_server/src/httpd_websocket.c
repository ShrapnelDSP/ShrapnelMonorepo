#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_httpd_priv.h"

#define TAG "websocket"

//the max size of output websocket frames in bytes this is the max size we can
//support right now, as we only use one byte to store the size
#define WEBSOCKET_BUFFER_SIZE 127

typedef struct {
    bool fin;
    uint8_t rsv123;
    enum {
        CONTINUATION_FRAME,
        TEXT_FRAME,
        BINARY_FRAME,
        CONNECTION_CLOSE_FRAME = 8,
        PING_FRAME,
        PONG_FRAME
    } opcode;
    bool mask;
    uint64_t payload_len;
    char *mask_key; /*<! Pointer to the first octet of the mask key */
    char *payload;  /*<! Pointer to the first octet of the payload  */
} websocket_frame_data_t;


static QueueHandle_t in_queue;

esp_err_t httpd_websocket_init(QueueHandle_t q)
{
    in_queue = q;
    return ESP_OK;
}

/* build binary data from websocket frame data struct */
static esp_err_t httpd_websocket_build(const websocket_frame_data_t *d, char *buf, size_t buf_len, size_t *output_size)
{
    if(d->mask)
    {
        ESP_LOGE(TAG, "Masking is not supported");
        return ESP_FAIL;
    }

    if(d->payload_len > 125)
    {
        ESP_LOGE(TAG, "Payload length over 125 is not supported");
        return ESP_FAIL;
    }

    //Payload plus 2 framing bytes must fit into the output buffer.
    //There can be more framing bytes if the payload size is larger than 125,
    //or if masking is enabled. However, neither of these is supported for now.
    if(buf_len < d->payload_len + 2)
    {
        ESP_LOGE(TAG, "Output buffer is too small");
        return ESP_FAIL;
    }

    memset(buf, 0, buf_len);

    buf[0] |= d->fin    << 7;
    buf[0] |= d->rsv123 << 4;
    buf[0] |= d->opcode;

    buf[1] |= d->mask << 7;
    buf[1] |= d->payload_len;
    
    memcpy(&buf[2], d->payload, d->payload_len);

    ESP_LOGD(TAG, "websocket frame build finished");
    ESP_LOG_BUFFER_HEXDUMP(TAG, buf, buf_len, ESP_LOG_DEBUG);

    *output_size = 2 + d->payload_len;
    return ESP_OK;
}

esp_err_t httpd_websocket_send_string(int fd, char *s)
{
    ESP_LOGD(TAG, "Sending string (%s)", s);

    char buf[WEBSOCKET_BUFFER_SIZE];
    size_t size;
    ssize_t sock_size;

    websocket_frame_data_t d = {
        .fin = true,
        .opcode = TEXT_FRAME,
        .mask = false,
    };

    d.payload_len = strlen(s);
    d.payload = s;

    esp_err_t ret = httpd_websocket_build(&d, buf, WEBSOCKET_BUFFER_SIZE, &size);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "websocket frame build failed");
        return ret;
    }

    sock_size = write(fd, buf, size);
    if(sock_size != size)
    {
        ESP_LOGE(TAG, "Socket error %d: %s", errno, strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

/* parse the data from a websocket frame and print it to the screen */
esp_err_t httpd_websocket_parse(httpd_handle_t hd, char *buf, size_t buf_len, int fd)
{
    esp_err_t err = ESP_OK;
    websocket_frame_data_t data = {0};

    //offset due to larger payload size
    int offset;
    uint32_t tmp;

    data.fin          = (buf[0] >> 7) & 0x01;
    data.rsv123       = (buf[0] >> 4) & 0x07;
    data.opcode       = (buf[0])      & 0x0f;

    data.mask         = (buf[1] >> 7) & 0x01;

    ESP_LOGD(TAG, "fin:\t0x%02x", data.fin);
    ESP_LOGD(TAG, "rsv123:\t0x%02x", data.rsv123);
    ESP_LOGD(TAG, "opcode:\t0x%02x", data.opcode);
    ESP_LOGD(TAG, "mask:\t0x%02x", data.mask);
    if(!data.mask)
    {
        ESP_LOGE(TAG, "Client should always mask");
        return ESP_FAIL;
    }

    tmp               = (buf[1])      & 0x7f;
    ESP_LOGD(TAG, "tmp_len:\t0x%02x", tmp);

    if(tmp == 126)
    {
        //the next two bytes are the extended length
        offset = 2;
    }
    else if(tmp == 127)
    {
        //the next 8 bytes are the extended length
        offset = 8;
    }
    else
    {
        //the extended length was not used
        data.payload_len = tmp;
        offset = 0;
    }

    for(int i = offset; i > 0; i--)
    {
        //TODO is this correct? Maybe use 64-bit ntoh if available
        ESP_LOGW(TAG, "Payload size was multi-byte, result might be wrong");
        data.payload_len |= buf[2 + i];
        data.payload_len <<= 8;
    }
    ESP_LOGD(TAG, "pl_len:\t0x%08llx", data.payload_len);
        
    data.mask_key = &buf[2 + offset];
    for(int i = 0; i < 4; i++)
    {
        ESP_LOGD(TAG, "key[%d]:\t0x%02x", i, data.mask_key[i]);
    }

    data.payload = &buf[6 + offset];

    switch(data.opcode)
    {
        case CONTINUATION_FRAME:
            ESP_LOGW(TAG, "Got continuation frame");
            break;
        case TEXT_FRAME:
            ESP_LOGI(TAG, "Got text data frame");

            /* send a copy of the payload to the input queue */
            char *copy = malloc(data.payload_len + 1);//+1 for the null terminator
            if(copy == NULL)
            {
                ESP_LOGE(TAG, "Couldn't malloc");
                return ESP_FAIL;
            }

            for(int i = 0; i < data.payload_len; i++)
            {
                char c;
                c = data.payload[i] ^ data.mask_key[i%4];
                ESP_LOGD(TAG, "dump: %c", c);

                if(c == '\0')
                {
                    ESP_LOGW(TAG, "Found NULL terminator in payload");
                }
                copy[i] = c;
            }
            copy[data.payload_len] = '\0';

            BaseType_t ret = xQueueSend(in_queue, &copy, pdMS_TO_TICKS(100));

            if(ret != pdPASS)
            {
                ESP_LOGE(TAG, "Output queue error, message dropped: %s", copy);
                free(copy);
                err = ESP_FAIL;
            }
            break;
        case BINARY_FRAME:
            ESP_LOGW(TAG, "Got binary data frame");
            break;
        case CONNECTION_CLOSE_FRAME:
            ESP_LOGW(TAG, "Got connection close frame");
            //TODO cleanly close the session associated with fd
            int n = shutdown(fd, SHUT_WR);
            if(n != 0)
                ESP_LOGE(TAG, "shutdown returned error %d, %s", errno, strerror(errno));

            do 
            {
                n = recv(fd, buf, buf_len, 0);
                ESP_LOGD(TAG, "recv returned %d", n);
                if(n < 0)
                    ESP_LOGE(TAG, "recv error %d, %s", errno, strerror(errno));
            } while (n != 0);

            n = close(fd);
            if(n != 0)
                ESP_LOGE(TAG, "close returned error %d, %s", errno, strerror(errno));

            break;
        case PING_FRAME:
            ESP_LOGW(TAG, "Got ping frame");
            break;
        case PONG_FRAME:
            ESP_LOGW(TAG, "Got pong frame");
            break;
        default:
            ESP_LOGE(TAG, "Got unknown opcode: %xh", data.opcode);
            break;
    }

    return err;
}

esp_err_t httpd_websocket_print_payload(websocket_frame_data_t *data)
{
    if(data->payload_len > 100)
    {
        ESP_LOGE(TAG, "payload length is too high, aborting (%lld)", data->payload_len);
        return ESP_FAIL;
    }

    for(int i = 0; i < data->payload_len; i++)
    {
        ESP_LOGD(TAG, "dump: %c", data->payload[i] ^ data->mask_key[i%4]);
    }
    return ESP_OK;
}
