
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"
#include "mdns.h"
#include "lwip/dns.h"
#include "websocket_server.h"
#include "websocket.h"
// #include "gps.h"
#include "connect_wifi.h"
#include "freertos/message_buffer.h"
MessageBufferHandle_t xMessageBufferToClient;
static const char *TAG = "MAIN";
static const char *MDNS_HOSTNAME = "ESP32";
int ws_server_start(void);
void udp_trans(void *pvParameters);
void server_task(void *pvParameters);
void client_task(void *pvParameters);
void app_main(void)
{
esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    connect_wifi();
    // dev._flip = true;
    // ESP_LOGW(TAG, "Flip upside down");

    // ESP_LOGI(TAG, "Panel is 128x64");
    // ssd1306_init(&dev, 128, 64);

    // ssd1306_contrast(&dev, 0xff);



    // while (1)
    // {
    //     ssd1306_clear_screen(&dev, false);
    //     ssd1306_display_text(&dev, 1, "BATMAN", 6, false);

    //     int bitmapWidth = 4 * 8;
    //     int width = ssd1306_get_width(&dev);
    //     int xpos = width / 2; // center of width
    //     xpos = xpos - bitmapWidth / 2;
    //     int ypos = 16;
    //     ESP_LOGD(TAG, "width=%d xpos=%d", width, xpos);
    //     ssd1306_bitmaps(&dev, xpos, ypos, batman, 32, 13, false);
    //     vTaskDelay(3000 / portTICK_PERIOD_MS);

    //     for (int i = 0; i < 128; i++)
    //     {
    //         ssd1306_wrap_arround(&dev, SCROLL_LEFT, 2, 3, 0);
            
    //     }
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);

    //     ssd1306_clear_screen(&dev, false);
    //     ssd1306_bitmaps(&dev, 0, 0, logoMischianti, 128, 64, false);
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);

    //     for (int i = 0; i < 64; i++)
    //     {
    //         ssd1306_wrap_arround(&dev, SCROLL_UP, 0, 127, 0);
    //     }
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);

    //     ssd1306_clear_screen(&dev, false);

    //     ssd1306_bitmaps(&dev, 0, 0, fleischer, 128, 64, false);
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }
}

