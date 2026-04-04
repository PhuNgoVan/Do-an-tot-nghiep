/*
 * mc7re.h
 *
 *  Created on: Mar 27, 2026
 *      Author: Asus
 */

#ifndef INC_MC7RE_H_
#define INC_MC7RE_H_

#ifdef __cplusplus
extern "C"
{
#endif

extern UART_HandleTypeDef 	huart1;
#define MC7RE_UART			huart1

typedef struct
{
	unsigned short CH1;
	unsigned short CH2;
	unsigned short CH3;
	unsigned short CH4;
	unsigned short CH5;
	unsigned short CH6;
	unsigned short CH7;
	unsigned short CH8;
}mc7re_channels;

unsigned short MC7RE_Get_valid(unsigned char CH_indx);

#ifdef __cplusplus
}
#endif


#endif /* INC_MC7RE_H_ */
