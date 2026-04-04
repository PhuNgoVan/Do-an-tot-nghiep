/*
 * BMP_280.h
 *
 *  Created on: Apr 4, 2026
 *      Author: Asus
 */

#ifndef INC_BMP_BMP_280_H_
#define INC_BMP_BMP_280_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

typedef struct
{
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;

    GPIO_TypeDef *mpu_ncs_port;
    uint16_t mpu_ncs_pin;

    int32_t t_fine;

    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    uint8_t chip_id;
    uint8_t is_ready;

} BMP280_Driver_t;

typedef struct
{
    int32_t temp_x100;     // ví dụ 2534 = 25.34°C
    uint32_t press_pa;     // áp suất Pa
} BMP280_RawValue_t;

void BMP280_DriverInit(BMP280_Driver_t *dev,
                       SPI_HandleTypeDef *hspi,
                       GPIO_TypeDef *cs_port,
                       uint16_t cs_pin,
                       GPIO_TypeDef *mpu_ncs_port,
                       uint16_t mpu_ncs_pin);

uint8_t BMP280_Init(BMP280_Driver_t *dev);
uint8_t BMP280_ReadChipID(BMP280_Driver_t *dev, uint8_t *id);
uint8_t BMP280_ReadRaw(BMP280_Driver_t *dev, BMP280_RawValue_t *out);


#ifdef __cplusplus
}
#endif

#endif /* INC_BMP_BMP_280_H_ */
