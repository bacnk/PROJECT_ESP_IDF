#ifndef GPS_H_
#define GPS_H_
#include <esp_system.h>
#include "stdint.h"
typedef struct{
    uint8_t hour, minute, second;
    char status;
    double latitude;
    char lat_dir;
    double longitude;
    char lon_dir;
    float speed;
    float course;
    uint8_t day, month, year;
}gps_info_t;

#define GPRMC_START_CHAR '$'
#define GPRNC_TYPE "GPRMC"
#define GPRMC_NUM_FIELDS 12
void process_gps_data(gps_info_t * gps_info)
#endif /* GPS_H_ */