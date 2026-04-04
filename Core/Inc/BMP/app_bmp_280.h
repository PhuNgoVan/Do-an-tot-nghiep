/*
 * app_bmp_280.h
 *
 *  Created on: Apr 4, 2026
 *      Author: Asus
 */

#ifndef INC_BMP_APP_BMP_280_H_
#define INC_BMP_APP_BMP_280_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "BMP/BMP_280.h"

typedef struct
{
    float temperature_c;
    float pressure_pa;
    float pressure_hpa;
} BMP280_AppData_t;

void BMP280_AppInit(void);
uint8_t BMP280_AppGetValue(BMP280_AppData_t *out);

#ifdef __cplusplus
}
#endif


#endif /* INC_BMP_APP_BMP_280_H_ */
