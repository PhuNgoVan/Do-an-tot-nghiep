/*
 * MPU_9250.h
 *
 *  Created on: Mar 28, 2026
 *      Author: Asus
 */

#ifndef INC_MPU_9250_H_
#define INC_MPU_9250_H_

#ifdef __cplusplus
extern "C"
{
#endif

extern SPI_HandleTypeDef 	hspi1;

// Pin cs
#define MPU_9250_SPI		hspi1
#define MPU_BMP_CSB_PORT	GPIOB
#define MPU_BMP_CSB_PIN		GPIO_PIN_1
#define MPU_9250_NCS_PORT	GPIOA
#define MPU_NCS_PIN			GPIO_PIN_4
#define MPU_W25_NCS_PORT	GPIOB
#define MPU_W25_NCS_PIN		GPIO_PIN_0

#define MPU9250_WHO_AM_I	0x75

unsigned char MPU9250_init(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_MPU_9250_H_ */
