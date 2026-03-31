/*
 * gps.c
 *
 *  Created on: Mar 31, 2026
 *      Author: Asus
 */

#include "GPS/gps.h"

static UART_HandleTypeDef *gps_huart = 0;

static uint8_t dma_buf[GPS_DRV_DMA_BUF_SIZE];
static volatile uint8_t ring_buf[GPS_DRV_RING_BUF_SIZE];
static volatile uint16_t ring_head = 0;
static volatile uint16_t ring_tail = 0;

static void GPS_PushByte(uint8_t data)
{
    uint16_t next = (uint16_t)((ring_head + 1U) % GPS_DRV_RING_BUF_SIZE);

    if (next != ring_tail)
    {
        ring_buf[ring_head] = data;
        ring_head = next;
    }
}

void GPS_Init(void)
{
    gps_huart = &GPS_UART;
    ring_head = 0;
    ring_tail = 0;
}

void GPS_Start(void)
{
    if (gps_huart != 0)
    {
        HAL_UARTEx_ReceiveToIdle_DMA(gps_huart, dma_buf, GPS_DRV_DMA_BUF_SIZE);
        __HAL_DMA_DISABLE_IT(gps_huart->hdmarx, DMA_IT_HT);
    }
}

void GPS_RxEventCallback(uint16_t size)
{
    uint16_t i;

	for (i = 0; i < size; i++)
	{
		GPS_PushByte(dma_buf[i]);
	}

	HAL_UARTEx_ReceiveToIdle_DMA(gps_huart, dma_buf, GPS_DRV_DMA_BUF_SIZE);
	__HAL_DMA_DISABLE_IT(gps_huart->hdmarx, DMA_IT_HT);
}

uint16_t GPS_Available(void)
{
    uint16_t count = 0;

    if (ring_head >= ring_tail)
    {
        count = ring_head - ring_tail;
    }
    else
    {
        count = GPS_DRV_RING_BUF_SIZE - ring_tail + ring_head;
    }

    return count;
}

uint8_t GPS_ReadByte(uint8_t *data)
{
    uint8_t ok = 0;

    if (ring_head != ring_tail)
    {
        *data = ring_buf[ring_tail];
        ring_tail = (uint16_t)((ring_tail + 1U) % GPS_DRV_RING_BUF_SIZE);
        ok = 1;
    }

    return ok;
}

