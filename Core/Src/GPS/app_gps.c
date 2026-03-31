/*
 * app_gps.c
 *
 *  Created on: Mar 31, 2026
 *      Author: Asus
 */
#include "GPS/app_gps.h"

static GPS_Data_t gps_data;
static char line_buf[GPS_LINE_MAX_LEN];
static uint16_t line_idx = 0;

static void GPS_App_ClearData(void)
{
    memset(&gps_data, 0, sizeof(gps_data));
}

static void GPS_App_CopyField(char *dst, uint16_t dst_size, const char *src)
{
    uint16_t i = 0;

    if (dst_size > 0U)
    {
        while ((src[i] != '\0') && (i < (dst_size - 1U)))
        {
            dst[i] = src[i];
            i++;
        }

        dst[i] = '\0';
    }
}

static void GPS_App_ParseRMC(const char *line)
{
    char temp[GPS_LINE_MAX_LEN];
    char *token;
    uint8_t field = 0;

    memset(temp, 0, sizeof(temp));
    strncpy(temp, line, sizeof(temp) - 1U);

    token = strtok(temp, ",");

    while (token != 0)
    {
        if (field == 1U)
        {
            GPS_App_CopyField(gps_data.utc_time, sizeof(gps_data.utc_time), token);
        }
        else if (field == 2U)
        {
            GPS_App_CopyField(gps_data.status, sizeof(gps_data.status), token);
        }
        else if (field == 3U)
        {
            GPS_App_CopyField(gps_data.latitude, sizeof(gps_data.latitude), token);
        }
        else if (field == 4U)
        {
            GPS_App_CopyField(gps_data.ns, sizeof(gps_data.ns), token);
        }
        else if (field == 5U)
        {
            GPS_App_CopyField(gps_data.longitude, sizeof(gps_data.longitude), token);
        }
        else if (field == 6U)
        {
            GPS_App_CopyField(gps_data.ew, sizeof(gps_data.ew), token);
        }
        else
        {
        }

        token = strtok(0, ",");
        field++;
    }

    GPS_App_CopyField(gps_data.raw_line, sizeof(gps_data.raw_line), line);
    gps_data.new_data = 1U;
}

static void GPS_App_ProcessLine(const char *line)
{
    if ((strncmp(line, "$GPRMC", 6U) == 0) || (strncmp(line, "$GNRMC", 6U) == 0))
    {
        GPS_App_ParseRMC(line);
    }
}

void GPS_App_Init(void)
{
    GPS_App_ClearData();
    memset(line_buf, 0, sizeof(line_buf));
    line_idx = 0;

    GPS_Init();
    GPS_Start();
}

void GPS_App_Task(void)
{
    uint8_t ch;
    uint8_t ok;

    ok = GPS_ReadByte(&ch);

    while (ok == 1U)
    {
        if (ch == '\r')
        {
        }
        else if (ch == '\n')
        {
            line_buf[line_idx] = '\0';

            if (line_idx > 0U)
            {
                GPS_App_ProcessLine(line_buf);
            }

            memset(line_buf, 0, sizeof(line_buf));
            line_idx = 0;
        }
        else
        {
            if (line_idx < (GPS_LINE_MAX_LEN - 1U))
            {
                line_buf[line_idx] = (char)ch;
                line_idx++;
            }
            else
            {
                memset(line_buf, 0, sizeof(line_buf));
                line_idx = 0;
            }
        }

        ok = GPS_ReadByte(&ch);
    }
}

void GPS_App_GetLatest(GPS_Data_t *out)
{
    if (out != 0)
    {
        memcpy(out, &gps_data, sizeof(GPS_Data_t));
        gps_data.new_data = 0U;
    }
}
