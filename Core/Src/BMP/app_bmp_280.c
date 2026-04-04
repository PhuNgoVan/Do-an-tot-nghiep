/*
 * app_bmp_280.c
 *
 *  Created on: Apr 4, 2026
 *      Author: Asus
 */
#include "BMP/app_bmp_280.h"

/*
 * Sửa lại đúng theo chân mày đang nối
 * Ví dụ:
 * BMP280 CSB  -> PB1
 * MPU9250 NCS -> PA4
 * SPI dùng hspi1
 */
extern SPI_HandleTypeDef hspi1;

static BMP280_Driver_t bmp_dev;

void BMP280_AppInit(void)
{
    BMP280_DriverInit(&bmp_dev,
                      &hspi1,
                      GPIOB, GPIO_PIN_1,   // BMP280 CSB
                      GPIOA, GPIO_PIN_4);  // MPU9250 NCS

    BMP280_Init(&bmp_dev);
}

uint8_t BMP280_AppGetValue(BMP280_AppData_t *out)
{
    uint8_t status;
    BMP280_RawValue_t raw;

    status = 0;

    if (BMP280_ReadRaw(&bmp_dev, &raw) != 0)
    {
        status = 1;
    }

    if (status == 0)
    {
        out->temperature_c = ((float)raw.temp_x100) / 100.0f;
        out->pressure_pa   = (float)raw.press_pa;
        out->pressure_hpa  = ((float)raw.press_pa) / 100.0f;
    }

    return status;
}

