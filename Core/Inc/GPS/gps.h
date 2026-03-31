/*
 * gps.c
 *
 *  Created on: Mar 31, 2026
 *      Author: Asus
 */

#ifndef INC_GPS_GPS_C_
#define INC_GPS_GPS_C_

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
extern UART_HandleTypeDef huart2;

#define GPS_UART				huart2
#define GPS_DRV_DMA_BUF_SIZE    128
#define GPS_DRV_RING_BUF_SIZE   512

void GPS_Init(void);
void GPS_Start(void);
void GPS_RxEventCallback(uint16_t size);

uint16_t GPS_Available(void);
uint8_t  GPS_ReadByte(uint8_t *data);

#ifdef __cplusplus
}
#endif
#endif /* INC_GPS_GPS_C_ */
