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

void process_gps_data(char *data)
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
        gps_info.hour = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
        printf("thoi gian %d\n", gps_info.hour);
        gps_info.minute = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
        gps_info.second = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
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
        gps_info.status = data[pos];

        valid = 1;
        // skip  ,
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
        // latitude
        length = 0;
        while (gpsMsg[pos + length] != '.')
            length++;
        gps_info.latitude = 0;
        length -= 2;
        for (i = 0; i < length; i++)
        {
            gps_info.latitude = gps_info.latitude * 10 + (gpsMsg[pos++] - '0');
        }
        i = 0;
        while (gpsMsg[pos] != ',')
            gpsConver[i++] = gpsMsg[pos++];
        pos++;
        gpsConver[i] = '\0';
        tempf = atof(gpsConver);
        gps_info.latitude += tempf / 60;
        gps_info.lat_dir = gpsMsg[pos];
        if (gpsMsg[pos] == 'S')
            gps_info.latitude = gps_info.latitude * -1;
        // skip  ,
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
        // longitude
        length = 0;
        while (gpsMsg[pos + length] != '.')
            length++;
        gps_info.longitude = 0;
        length -= 2;
        for (i = 0; i < length; i++)
        {
            gps_info.longitude = gps_info.longitude * 10 + (gpsMsg[pos++] - '0');
        }
        i = 0;
        while (gpsMsg[pos] != ',')
            gpsConver[i++] = gpsMsg[pos++];
        pos++;
        gpsConver[i] = '\0';
        tempf = atof(gpsConver);
        gps_info.longitude += tempf / 60;
        gps_info.lon_dir = gpsMsg[pos];
        if (gpsMsg[pos] == 'S')
            gps_info.longitude = gps_info.longitude * -1;
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
        gps_info.speed = temp & 0xFFFF;
        // COG sensor
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
        i = 0;
        while (gpsMsg[pos] != ',')
            gpsConver[i++] = gpsMsg[pos++];
        gpsConver[i] = '\0';
        gpsInfo.course = atof(gpsConver);

        // day month year
        while (gpsMsg[pos] != ',')
            pos++;
        pos++;
         gps_info.day = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
        printf("thoi gian %d\n", gps_info.hour);
        gps_info.month = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
        gps_info.year = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
          if (gps_info.speed >= 180)
        {
            gps_info.speed = 0;
            // Phân tích xong
            return;
        }
    }
}