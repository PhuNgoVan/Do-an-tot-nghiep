/*
 * BMP_280.c
 *
 *  Created on: Apr 4, 2026
 *      Author: Asus
 */

#include "BMP/BMP_280.h"

#define BMP280_REG_ID            0xD0
#define BMP280_REG_RESET         0xE0
#define BMP280_REG_STATUS        0xF3
#define BMP280_REG_CTRL_MEAS     0xF4
#define BMP280_REG_CONFIG        0xF5
#define BMP280_REG_PRESS_MSB     0xF7
#define BMP280_REG_CALIB00       0x88

#define BMP280_CHIP_ID_VALUE     0x58
#define BMP280_RESET_VALUE       0xB6

static void BMP280_Select(BMP280_Driver_t *dev)
{
    HAL_GPIO_WritePin(dev->mpu_ncs_port, dev->mpu_ncs_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
}

static void BMP280_Deselect(BMP280_Driver_t *dev)
{
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}

static uint8_t BMP280_ReadRegs(BMP280_Driver_t *dev, uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint8_t status;
    uint8_t addr;

    status = 0;
    addr = reg | 0x80;

    BMP280_Select(dev);

    if (HAL_SPI_Transmit(dev->hspi, &addr, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        status = 1;
    }

    if (status == 0)
    {
        if (HAL_SPI_Receive(dev->hspi, buf, len, HAL_MAX_DELAY) != HAL_OK)
        {
            status = 1;
        }
    }

    BMP280_Deselect(dev);

    return status;
}

static uint8_t BMP280_WriteReg(BMP280_Driver_t *dev, uint8_t reg, uint8_t value)
{
    uint8_t status;
    uint8_t tx[2];

    status = 0;
    tx[0] = reg & 0x7F;
    tx[1] = value;

    BMP280_Select(dev);

    if (HAL_SPI_Transmit(dev->hspi, tx, 2, HAL_MAX_DELAY) != HAL_OK)
    {
        status = 1;
    }

    BMP280_Deselect(dev);

    return status;
}

static uint16_t BMP280_U16_LE(uint8_t *b)
{
    uint16_t v;
    v = ((uint16_t)b[1] << 8) | b[0];
    return v;
}

static int16_t BMP280_S16_LE(uint8_t *b)
{
    int16_t v;
    v = (int16_t)(((uint16_t)b[1] << 8) | b[0]);
    return v;
}

static uint8_t BMP280_ReadCalib(BMP280_Driver_t *dev)
{
    uint8_t status;
    uint8_t calib[24];

    status = 0;

    if (BMP280_ReadRegs(dev, BMP280_REG_CALIB00, calib, 24) != 0)
    {
        status = 1;
    }

    if (status == 0)
    {
        dev->dig_T1 = BMP280_U16_LE(&calib[0]);
        dev->dig_T2 = BMP280_S16_LE(&calib[2]);
        dev->dig_T3 = BMP280_S16_LE(&calib[4]);

        dev->dig_P1 = BMP280_U16_LE(&calib[6]);
        dev->dig_P2 = BMP280_S16_LE(&calib[8]);
        dev->dig_P3 = BMP280_S16_LE(&calib[10]);
        dev->dig_P4 = BMP280_S16_LE(&calib[12]);
        dev->dig_P5 = BMP280_S16_LE(&calib[14]);
        dev->dig_P6 = BMP280_S16_LE(&calib[16]);
        dev->dig_P7 = BMP280_S16_LE(&calib[18]);
        dev->dig_P8 = BMP280_S16_LE(&calib[20]);
        dev->dig_P9 = BMP280_S16_LE(&calib[22]);
    }

    return status;
}

static int32_t BMP280_CompensateTemp(BMP280_Driver_t *dev, int32_t adc_T)
{
    int32_t var1;
    int32_t var2;
    int32_t T;

    var1 = ((((adc_T >> 3) - ((int32_t)dev->dig_T1 << 1))) * ((int32_t)dev->dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dev->dig_T1)) * ((adc_T >> 4) - ((int32_t)dev->dig_T1))) >> 12) *
            ((int32_t)dev->dig_T3)) >> 14;

    dev->t_fine = var1 + var2;
    T = (dev->t_fine * 5 + 128) >> 8;

    return T;
}

static uint32_t BMP280_CompensatePress(BMP280_Driver_t *dev, int32_t adc_P)
{
    int64_t var1;
    int64_t var2;
    int64_t p;
    uint32_t press;

    press = 0;

    var1 = ((int64_t)dev->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dev->dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dev->dig_P3) >> 8) + ((var1 * (int64_t)dev->dig_P2) << 12);
    var1 = ((((int64_t)1 << 47) + var1) * ((int64_t)dev->dig_P1)) >> 33;

    if (var1 != 0)
    {
        p = 1048576 - adc_P;
        p = (((p << 31) - var2) * 3125) / var1;
        var1 = (((int64_t)dev->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        var2 = (((int64_t)dev->dig_P8) * p) >> 19;
        p = ((p + var1 + var2) >> 8) + (((int64_t)dev->dig_P7) << 4);
        press = (uint32_t)(p >> 8);
    }

    return press;
}

void BMP280_DriverInit(BMP280_Driver_t *dev,
                       SPI_HandleTypeDef *hspi,
                       GPIO_TypeDef *cs_port,
                       uint16_t cs_pin,
                       GPIO_TypeDef *mpu_ncs_port,
                       uint16_t mpu_ncs_pin)
{
    dev->hspi = hspi;
    dev->cs_port = cs_port;
    dev->cs_pin = cs_pin;
    dev->mpu_ncs_port = mpu_ncs_port;
    dev->mpu_ncs_pin = mpu_ncs_pin;

    dev->t_fine = 0;
    dev->chip_id = 0;
    dev->is_ready = 0;

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(dev->mpu_ncs_port, dev->mpu_ncs_pin, GPIO_PIN_SET);
}

uint8_t BMP280_ReadChipID(BMP280_Driver_t *dev, uint8_t *id)
{
    uint8_t status;
    uint8_t rx;

    status = 0;
    rx = 0;

    if (BMP280_ReadRegs(dev, BMP280_REG_ID, &rx, 1) != 0)
    {
        status = 1;
    }

    if (status == 0)
    {
        dev->chip_id = rx;
        *id = rx;
    }

    return status;
}

uint8_t BMP280_Init(BMP280_Driver_t *dev)
{
    uint8_t status;
    uint8_t id;

    status = 0;
    id = 0;

    dev->is_ready = 0;

    if (BMP280_ReadChipID(dev, &id) != 0)
    {
        status = 1;
    }

    if (status == 0)
    {
        if (id != BMP280_CHIP_ID_VALUE)
        {
            status = 1;
        }
    }

    if (status == 0)
    {
        if (BMP280_WriteReg(dev, BMP280_REG_RESET, BMP280_RESET_VALUE) != 0)
        {
            status = 1;
        }
    }

    if (status == 0)
    {
        HAL_Delay(10);

        if (BMP280_ReadCalib(dev) != 0)
        {
            status = 1;
        }
    }

    if (status == 0)
    {
        /*
         * ctrl_meas:
         * osrs_t = x1   -> 001
         * osrs_p = x1   -> 001
         * mode   = normal -> 11
         * => 0b00100111 = 0x27
         */
        if (BMP280_WriteReg(dev, BMP280_REG_CTRL_MEAS, 0x27) != 0)
        {
            status = 1;
        }
    }

    if (status == 0)
    {
        /*
         * config:
         * standby 500 ms, filter off, spi 4-wire
         * 0xA0
         */
        if (BMP280_WriteReg(dev, BMP280_REG_CONFIG, 0xA0) != 0)
        {
            status = 1;
        }
    }

    if (status == 0)
    {
        dev->is_ready = 1;
    }

    return status;
}

uint8_t BMP280_ReadRaw(BMP280_Driver_t *dev, BMP280_RawValue_t *out)
{
    uint8_t status;
    uint8_t raw[6];
    int32_t adc_p;
    int32_t adc_t;

    status = 0;
    adc_p = 0;
    adc_t = 0;

    if (dev->is_ready == 0)
    {
        status = 1;
    }

    if (status == 0)
    {
        if (BMP280_ReadRegs(dev, BMP280_REG_PRESS_MSB, raw, 6) != 0)
        {
            status = 1;
        }
    }

    if (status == 0)
    {
        adc_p = ((int32_t)raw[0] << 12) | ((int32_t)raw[1] << 4) | ((int32_t)(raw[2] >> 4));
        adc_t = ((int32_t)raw[3] << 12) | ((int32_t)raw[4] << 4) | ((int32_t)(raw[5] >> 4));

        out->temp_x100 = BMP280_CompensateTemp(dev, adc_t);
        out->press_pa = BMP280_CompensatePress(dev, adc_p);
    }

    return status;
}
