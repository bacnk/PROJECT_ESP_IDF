#include "gps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utilities.h"
// Hàm xử lý dữ liệu GPS
extern gps_info_t gps_info;
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

void process_gps_data(void)
{
    uint8_t pos = 0, length = 0, i = 0;
    double tempf = 0;
    char gpsConver[12];
    
    if(gpsMsg != NULL)
    {

    }
}