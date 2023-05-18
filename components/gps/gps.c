#include "gps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "Utilities.h"
//
#define GGA 0x00
#define GGL 0x01
#define GSA 0x02
#define GSV 0x03
#define RMC 0x04
#define VTG 0x05
static uint8_t NewGPSData = 0;
static uint8_t SendOnlyGPRMC = 0;
static const int RX_BUF_SIZE = 1024;
uint8_t UBX_Message[20];
uint8_t UBX_Index;
uint8_t MSG_ID[6] = {GGA, GGL, GSA, GSV, VTG};
static const char *TAG = "GPS_process";
// Hàm xử lý dữ liệu GPS
extern gps_info_t gps_info;
void init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = GPS_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    static const gps_info_t initial_gps_info = {0};           // Initialize gps_info with all fields set to 0
    memcpy(&gps_info, &initial_gps_info, sizeof(gps_info_t)); // Copy initial_gps_info to gps_info

    uart_driver_install(GPS_UART_PORT, GPS_UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(GPS_UART_PORT, &uart_config);
    uart_set_pin(GPS_UART_PORT, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
void receive_gps_data(void *pvParameters)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE);
    int len = uart_read_bytes(UART, data, RX_BUF_SIZE, 100 / portTICK_RATE_MS);
    if (len > 0)
    {
        data[len] = '\0';
        ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", len, data);
        ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, len, ESP_LOG_INFO);
        process_gps_data((char *)data, &gps_info);
        ESP_LOGI(TAG, "Hour: %d", gps_info.hour);
        ESP_LOGI(TAG, "Minute: %d", gps_info.minute);
        ESP_LOGI(TAG, "Second: %d", gps_info.second);
        ESP_LOGI(TAG, "Status: %c", gps_info.status);
        ESP_LOGI(TAG, "Latitude: %lf", gps_info.latitude);
        ESP_LOGI(TAG, "Latitude Direction: %c", gps_info.lat_dir);
        ESP_LOGI(TAG, "Longitude: %lf", gps_info.longitude);
        ESP_LOGI(TAG, "Longitude Direction: %c", gps_info.lon_dir);
        ESP_LOGI(TAG, "Speed: %f", gps_info.speed);
        ESP_LOGI(TAG, "Course: %f", gps_info.course);
        ESP_LOGI(TAG, "Day: %d", gps_info.day);
        ESP_LOGI(TAG, "Month: %d", gps_info.month);
        ESP_LOGI(TAG, "Year: %d", gps_info.year);
    }
    free(data);
}
uint8_t validCheckSum(char *gpsdata)
{
    uint8_t GPSCRC = 0;
    uint8_t messageCRC = 0;
    uint8_t i = 0;
    messageCRC = 0;
    for (i = 1; i < strlen(gpsdata); i++)
    {
        if (gpsdata[i] == '*')
        {
            messageCRC = 0xFF;
            break;
        }
        GPSCRC ^= gpsdata[i];
    }
    if (messageCRC != 0xFF)
    {
        return 0;
    }
    messageCRC = getNumberFromString(i + 1, gpsdata) && 0XFF;
    if (messageCRC == GPSCRC)
    {
        return 1;
    }
    return 0;
}

void process_gps_data(char *data, gps_info_t *gps_info)
{
    uint8_t pos = 0, length = 0, i = 0, valid = 0;
    double tempf = 0;
    uint32_t temp = 0;
    char gpsConver[200];

    char *gpsMsg = strstr((char *)&data[pos], "$GPRMC");
    if (gpsMsg != NULL)
    {
        if (validCheckSum(gpsMsg) == 0)
            return;
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;

        // hour:minute:second
        gps_info->hour = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
        printf("thoi gian %d\n", gps_info->hour);
        gps_info->minute = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
        gps_info->second = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
        // skip ,
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
        // validity
        if (gpsMsg[pos] != 'A')
        {
            valid = 0;
            return;
        }
        gps_info->status = data[pos];

        valid = 1;
        // skip  ,
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
        // latitude
        length = 0;
        while (gpsMsg[pos + length] != '.')
            length++;
        gps_info->latitude = 0;
        length -= 2;
        for (i = 0; i < length; i++)
        {
            gps_info->latitude = gps_info->latitude * 10 + (gpsMsg[pos++] - '0');
        }
        i = 0;
        while (gpsMsg[pos] != ',')
            gpsConver[i++] = gpsMsg[pos++];
        pos++;
        gpsConver[i] = '\0';
        tempf = atof(gpsConver);
        gps_info->latitude += tempf / 60;
        gps_info->lat_dir = gpsMsg[pos];
        if (gpsMsg[pos] == 'S')
            gps_info->latitude = gps_info->latitude * -1;
        // skip  ,
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
        // longitude
        length = 0;
        while (gpsMsg[pos + length] != '.')
            length++;
        gps_info->longitude = 0;
        length -= 2;
        for (i = 0; i < length; i++)
        {
            gps_info->longitude = gps_info->longitude * 10 + (gpsMsg[pos++] - '0');
        }
        i = 0;
        while (gpsMsg[pos] != ',')
            gpsConver[i++] = gpsMsg[pos++];
        pos++;
        gpsConver[i] = '\0';
        tempf = atof(gpsConver);
        gps_info->longitude += tempf / 60;
        gps_info->lon_dir = gpsMsg[pos];
        if (gpsMsg[pos] == 'S')
            gps_info->longitude = gps_info->longitude * -1;
        // skip ,
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
        // knot to km/h
        temp = getNumberFromString(pos, (char *)gpsMsg);
        temp = temp * 1000;
        while (gpsMsg[pos] != '.')
            pos++;
        pos++;
        temp += getNumberFromString(pos, (char *)gpsMsg);
        temp = temp * 13;
        temp = temp / 7020;
        gps_info->speed = temp & 0xFFFF;
        // COG sensor
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
        i = 0;
        while (gpsMsg[pos] != ',')
            gpsConver[i++] = gpsMsg[pos++];
        gpsConver[i] = '\0';
        gps_info->course = atof(gpsConver);

        // day month year
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
        gps_info->day = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;

        gps_info->month = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
        gps_info->year = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
        if (gps_info->speed >= 180)
        {
           gps_info->speed = 0;
            // Phân tích xong
            return;
        }
        // ESP_LOGE(TAG, "Time: %02d:%02d:%02d\n", gps_info.hour, gps_info.minute,
        //          gps_info.second);
        // ESP_LOGE(TAG, "Status: %c\n", gps_info.status);
        // ESP_LOGE(TAG, "Latitude: %.5lf %c\n", gps_info.latitude, gps_info.lat_dir);
        // ESP_LOGE(TAG, "longitude: %.5lf %c\n", gps_info.longitude, gps_info.lon_dir);
        // ESP_LOGE(TAG, "SPEDD: %.2f\n", gps_info.speed);
        // ESP_LOGE(TAG, "course: %.2f\n", gps_info.course);
        // ESP_LOGE(TAG, "date: %02d-%02d-%02d\n", gps_info.day, gps_info.month,
        //          gps_info.year);
    }
}
static void UBX_MessConfig(uint8_t MsgID, uint8_t outputRate)
{
    static const char *TX = "GPS_TX";
    UBX_Index = 0;

    // HEADER
    UBX_Message[UBX_Index++] = 0xB5;
    UBX_Message[UBX_Index++] = 0x62;
    // CLASS FIELD
    UBX_Message[UBX_Index++] = 0x06;
    // ID FIELD
    UBX_Message[UBX_Index++] = 0x01;
    // PAYLOAD - 3 BYTES
    UBX_Message[UBX_Index++] = 3;
    UBX_Message[UBX_Index++] = 0;

    //  PAYLOAD - 3 BYTES
    // RMC MSG
    UBX_Message[UBX_Index++] = 0xF0;
    UBX_Message[UBX_Index++] = MsgID;
    UBX_Message[UBX_Index++] = outputRate;
    // CHECKSUM - 2
    uint8_t CK_A = 0, CK_B = 0;
    for (uint8_t i = 0; i < UBX_Index - 2; i++)
    {
        CK_A = CK_A + UBX_Message[i + 2];
        CK_B = CK_B + CK_A;
    }
    UBX_Message[UBX_Index++] = CK_A & 0xFF;
    UBX_Message[UBX_Index++] = CK_B & 0xFF;

    // Send UBX_Message over UART
    uart_write_bytes(UART, UBX_Message, UBX_Index);
    ESP_LOGE(TX, "Transmit GPS data: %s", UBX_Message);
}
void GPS_ManagerTask(void)
{
    static uint8_t GPS_TickCount = 0;
    if (SendOnlyGPRMC < 5)
    {
    }
}