#ifndef GPS_H_
#define GPS_H_

#include <stdint.h>
#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#define RX_PIN GPIO_NUM_22
#define TX_PIN GPIO_NUM_23
#define GPS_UART_PORT UART_NUM_2
#define GPS_UART_BAUDRATE 9600
#define GPS_UART_BUF_SIZE 1024

#define UART UART_NUM_2
// Khai báo kiểu dữ liệu để lưu trữ thông tin GPS
typedef struct
{
    uint8_t hour;     // Giờ
    uint8_t minute;   // Phút
    uint8_t second;   // Giây
    char status;      // Trạng thái GPS (A: khả dụng, V: không khả dụng)
    double latitude;  // Vĩ độ
    char lat_dir;     // Hướng vĩ độ (N: Bắc, S: Nam)
    double longitude; // Kinh độ
    char lon_dir;     // Hướng kinh độ (E: Đông, W: Tây)
    float speed;      // Tốc độ (đơn vị: knot)
    float course;     // Phương hướng (đơn vị: độ)
    uint8_t day;      // Ngày
    uint8_t month;    // Tháng
    uint16_t year;    // Năm
} gps_info_t;

// Khai báo hằng số cho bản tin GPRMC
#define GPRMC_START_CHAR '$'
#define GPRMC_TYPE "GPRMC"
#define GPRMC_NUM_FIELDS 12

// Định nghĩa hàm xử lý dữ liệu GPS

void init(void);
void process_gps_data(char *data, gps_info_t *gps_info);
#endif /* GPS_H_ */
