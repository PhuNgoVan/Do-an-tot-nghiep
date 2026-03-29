/*
 * MP_9250.c
 *
 *  Created on: Mar 28, 2026
 *      Author: Asus
 */
#include "main.h"
#include "MPU_9250.h"

static void MPU9250_enable(void)
{
	HAL_GPIO_WritePin(MPU_BMP_CSB_PORT, MPU_BMP_CSB_PIN, 1);
	HAL_Delay(100);
	HAL_GPIO_WritePin(MPU_9250_NCS_PORT, MPU_NCS_PIN, 0);
}

static void MPU9250_Disable(void)
{
	HAL_GPIO_WritePin(MPU_9250_NCS_PORT, MPU_NCS_PIN, 1);
}

static void MPU9250_ReadRegs(uint8_t reg, uint8_t *buf, uint16_t len)
{
    unsigned char addr = reg | 0x80;

    HAL_SPI_Transmit(&MPU_9250_SPI, &addr, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&MPU_9250_SPI, buf, len, HAL_MAX_DELAY);
}

static void MPU9250_WriteReg(uint8_t reg, uint8_t data)
{
	unsigned char tx[2];

    tx[0] = reg & 0x7F;
    tx[1] = data;

    HAL_SPI_Transmit(&hspi1, tx, 2, HAL_MAX_DELAY);
}

static unsigned char MPU9250_ReadReg(uint8_t reg)
{
	unsigned char tx[2];
	unsigned char rx[2];

    tx[0] = reg | 0x80;
    tx[1] = 0x00;
    rx[0] = 0;
    rx[1] = 0;

	MPU9250_enable();
	HAL_SPI_Transmit(&MPU_9250_SPI, tx, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&MPU_9250_SPI, rx, 1, HAL_MAX_DELAY);
    MPU9250_Disable();

    return rx[1];
}

unsigned char MPU9250_init(void)
{
	unsigned char who;

	HAL_GPIO_WritePin(MPU_BMP_CSB_PORT, MPU_BMP_CSB_PIN, 1);
	HAL_GPIO_WritePin(MPU_9250_NCS_PORT, MPU_NCS_PIN, 1);
	HAL_Delay(10);

	who = MPU9250_ReadReg(MPU9250_WHO_AM_I);
    if (who != 0x71)
    {
        who = 0;
    }

    HAL_Delay(100);

	who = MPU9250_ReadReg(MPU9250_WHO_AM_I);
    if (who != 0x71)
    {
        who = 0;
    }

    MPU9250_Disable();
    return who;
}

