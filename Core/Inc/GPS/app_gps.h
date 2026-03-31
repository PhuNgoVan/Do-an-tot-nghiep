/*
 * app_gps.h
 *
 *  Created on: Mar 31, 2026
 *      Author: Asus
 */

#ifndef INC_GPS_APP_GPS_H_
#define INC_GPS_APP_GPS_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "GPS/gps.h"

#define GPS_LINE_MAX_LEN  128

/* Module Gps connect to 4 satellites to get data
Module send data frequency so that use dma and ring buffer ro store data*/
typedef struct
{
    char raw_line[GPS_LINE_MAX_LEN];

    char utc_time[16];
    char status[4];
    char latitude[16];
    char ns[4];
    char longitude[16];
    char ew[4];

    uint8_t new_data;
} GPS_Data_t;

void GPS_App_Init(void);
void GPS_App_Task(void);
void GPS_App_GetLatest(GPS_Data_t *out);

#ifdef __cplusplus
}
#endif
#endif /* INC_GPS_APP_GPS_H_ */
