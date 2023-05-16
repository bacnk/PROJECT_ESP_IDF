#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint8_t hour;   // Giờ
  uint8_t minute; // Phút
  uint8_t second; // Giây
  char status;    // Trạng thái GPS (A: khả dụng, V: không khả dụng)
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
uint32_t getNumberFromString(uint16_t BeginAddress, char *Buffer) {

  uint16_t i = 0;
  i = BeginAddress;
  uint32_t hexNumber = 0;

  // Skip any leading white space
  while (isspace(Buffer[i])) {
    i++;
  }

  // Skip "0x" prefix if present
  if (Buffer[i] == '0' && (Buffer[i + 1] == 'x' || Buffer[i + 1] == 'X')) {
    i += 2;
  }

  // Parse the hex number
  while (isxdigit(Buffer[i])) {
    char c = tolower(Buffer[i]);
    uint8_t nibble = (c >= 'a' && c <= 'f') ? (c - 'a' + 10) : (c - '0');
    hexNumber = (hexNumber << 4) | nibble;
    i++;
  }

  return hexNumber;
}
void process_gps_data(char *data, gps_info_t *gps_info) {
  uint8_t pos = 0, length = 0, i = 0, valid = 0;
  double tempf = 0;
  uint32_t temp = 0;
  char gpsConver[200];

  char *gpsMsg = strstr(data, "$GPRMC");
  if (gpsMsg != NULL) {
    pos += 6; // Skip "$GPRMC"
    while (gpsMsg[pos] != ',')
      pos++;
    pos++;
    // Extract hour
    gps_info->hour = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
    pos += 2;

    // Extract minute
    gps_info->minute = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
    pos += 2;

    // Extract second
    gps_info->second = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
    pos += 2;
    //
    while (gpsMsg[pos] != ',')
      pos++;
    pos++;
    // Check GPS status
    if (data[pos] != 'A') {
      valid = 0;
      return;
    }
    gps_info->status = data[pos];
    valid = 1;
    while (gpsMsg[pos] != ',')
      pos++;
    pos++;

    length = 0;
    while (gpsMsg[pos + length] != '.')
      length++;
    gps_info->latitude = 0;
    length -= 2;
    for (i = 0; i < length; i++) {
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

    //////////
    while (gpsMsg[pos] != ',')
      pos++;
    pos++;
    // longitude
    length = 0;
    while (gpsMsg[pos + length] != '.')
      length++;
    gps_info->longitude = 0;
    length -= 2;
    for (i = 0; i < length; i++) {
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
        //////////// 
    while (gpsMsg[pos] != ',')
        pos++;
    pos++; 
    i=0;
    while (gpsMsg[pos] != ',')
    gpsConver[i++] = gpsMsg[pos++];
    gpsConver[i] ='\0';
      gps_info->course = atof(gpsConver);
      // 
      while (gpsMsg[pos] != ',')
            pos++;
        pos++;
         gps_info->day = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
         gps_info->month = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
         gps_info->year = (data[pos] - '0') * 10 + (data[pos + 1] - '0');
        pos += 2;
          if ( gps_info->speed >= 180)
        {
            gps_info->speed = 0;
            // Phân tích xong
            return;
        }
  }
}

int main() {
  char gps_data[] =
      "$GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70";

  gps_info_t gps_info;
  process_gps_data(gps_data, &gps_info);

  printf("GPS Data:\n");
  printf("Time: %02d:%02d:%02d\n", gps_info.hour, gps_info.minute,
         gps_info.second);
  printf("Status: %c\n", gps_info.status);
  printf("Latitude: %.5lf %c\n", gps_info.latitude, gps_info.lat_dir);
  printf("longitude: %.5lf %c\n", gps_info.longitude, gps_info.lon_dir);
  printf("SPEDD: %.2f\n", gps_info.speed);
    printf("course: %.2f\n", gps_info.course);
    printf("date: %02d-%02d-%02d\n", gps_info.day, gps_info.month,
         gps_info.year);

  return 0;
}
