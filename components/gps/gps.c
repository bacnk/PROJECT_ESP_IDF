#include "gps.h"
#include "string.h"
void process_gps_data(gps_info_t* gps_info,const char* gps_data)
{
    const char* pch = strstr(gps_data, "GPRMC");
    if(pch == NULL)
    {
        return;
    }
    pch +=7;
    uint8_t i=0;
    char * field;
    pch +=7;
    while ((field = strsep((char**)&pch, " ")) != NULL)
 switch (i) {
        case 0:
            // Giờ
            if (strlen(field) >= 6) {
                sscanf(field, "%2hhu%2hhu%2hhu", &gps_info->hour, &gps_info->minute, &gps_info->second);
            }
            break;
        case 1:
            // Trạng thái GPS (A: khả dụng, V: không khả dụng)
            gps_info->status = field[0];
            break;
        case 2:
            // Vĩ độ
            gps_info->latitude = strtod(field, NULL);
            break;
        case 3:
            // Hướng vĩ độ (N: Bắc, S: Nam)
            gps_info->lat_dir = field[0];
            break;
        case 4:
            // Kinh độ
            gps_info->longitude = strtod(field, NULL);
            break;
        case 5:
            // Hướng kinh độ (E: Đông, W: Tây)
            gps_info->lon_dir = field[0];
            break;
        case 6:
            // Tốc độ (đơn vị: knot)
            gps_info->speed = strtof(field, NULL);
            break;
        case 7:
            // Phương hướng (đơn vị: độ)
            gps_info->course = strtof(field, NULL);
            break;
        case 8:
            // Ngày
            if (strlen(field) >= 6) {
                sscanf(field, "%2hhu%2hhu%2hhu", &gps_info->day, &gps_info->month, &gps_info->year);
                gps_info->year += 2000;
            }
            break;
        default:
            break;
        }
        i++;
        if (i >= GPRMC_NUM_FIELDS) {
            // Đã phân tích đủ số trường trong bản tin GPRMC
            break;
        }
    }
    
