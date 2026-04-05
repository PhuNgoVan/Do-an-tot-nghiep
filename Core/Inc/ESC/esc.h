/*
 * esc.h
 *
 *  Created on: Apr 5, 2026
 *      Author: Asus
 */

#ifndef INC_ESC_ESC_H_
#define INC_ESC_ESC_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

void ESC_Start(void);
void ESC_Arm(void);
void ESC_SetUs(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);
void ESC_SetAll(uint16_t throttle_us);

#ifdef __cplusplus
}
#endif
#endif /* INC_ESC_ESC_H_ */
