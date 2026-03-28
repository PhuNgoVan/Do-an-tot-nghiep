/*
 * mc7re.h
 *
 *  Created on: Mar 27, 2026
 *      Author: Asus
 */

#ifndef SRC_MC7RE_C_
#define SRC_MC7RE_C_

#include "main.h"
#include "mc7re.h"

static mc7re_channels 	mc7re;

static unsigned char 	buffer[100];
static uint8_t 			sbus_idx = 0;
static uint8_t 			rx_byte;

static void SBUS_Parse(uint8_t *b)
{
	mc7re.CH1  = ((b[1]      | b[2]  << 8) & 0x07FF);
	mc7re.CH2  = ((b[2] >> 3 | b[3]  << 5) & 0x07FF);
	mc7re.CH3  = ((b[3] >> 6 | b[4]  << 2 | b[5] << 10) & 0x07FF);
	mc7re.CH4  = ((b[5] >> 1 | b[6]  << 7) & 0x07FF);
	mc7re.CH5  = ((b[6] >> 4 | b[7]  << 4) & 0x07FF);
	mc7re.CH6  = ((b[7] >> 7 | b[8]  << 1 | b[9] << 9) & 0x07FF);
	mc7re.CH7  = ((b[9] >> 2 | b[10] << 6) & 0x07FF);
    mc7re.CH8  = ((b[10]>> 5 | b[11] << 3) & 0x07FF);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (huart->Instance == USART1)
        {
            if (sbus_idx == 0)
            {
                if (rx_byte == 0x0F)
                {
                    buffer[sbus_idx++] = rx_byte;
                }
            }
            else
            {
                buffer[sbus_idx++] = rx_byte;

                if (sbus_idx >= 25)
                {
                    SBUS_Parse(buffer);
                    sbus_idx = 0;
                }
            }

            HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
        }
    }
}

unsigned short MC7RE_Get_valid(unsigned char ch)
{
	HAL_UART_Receive_IT(&MC7RE_UART, &buffer, 1);
    switch (ch)
    {
        case 1: return mc7re.CH1;
        case 2: return mc7re.CH2;
        case 3: return mc7re.CH3;
        case 4: return mc7re.CH4;
        case 5: return mc7re.CH5;
        case 6: return mc7re.CH6;
        case 7: return mc7re.CH7;
        case 8: return mc7re.CH8;
        default: return 0;
    }

}

#endif /* SRC_MC7RE_C_ */
