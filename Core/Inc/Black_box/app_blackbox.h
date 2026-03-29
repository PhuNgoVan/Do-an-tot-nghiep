/*
 * app_blackbox.h
 *
 *  Created on: Mar 29, 2026
 *      Author: Asus
 */

#ifndef INC_BLACK_BOX_APP_BLACKBOX_H_
#define INC_BLACK_BOX_APP_BLACKBOX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Black_box/w25q128.h"

#define APP_BB_MAGIC             0x4242AA55UL
#define APP_BB_MAX_USER_DATA     128U

typedef enum
{
    APP_BB_OK = 0,
    APP_BB_ERR,
    APP_BB_FULL,
    APP_BB_INVALID_PARAM,
    APP_BB_NOT_INIT
} APP_BB_Status_t;

typedef struct __attribute__((packed))
{
    uint32_t magic;
    uint32_t seq;
    uint32_t tick_ms;
    uint8_t  type;
    uint16_t data_len;
    uint16_t crc;
} APP_BB_RecordInfo_t;

typedef struct
{
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t write_addr;
    uint32_t seq;
    uint8_t  initialized;
} APP_BB_Handle_t;

APP_BB_Status_t APP_BB_Init(APP_BB_Handle_t *bb, uint32_t start_addr, uint32_t end_addr);
APP_BB_Status_t APP_BB_Log(APP_BB_Handle_t *bb, uint8_t type, const uint8_t *data, uint16_t len);
APP_BB_Status_t APP_BB_ReadRecord(APP_BB_Handle_t *bb,
                                  uint32_t addr,
                                  APP_BB_RecordInfo_t *info,
                                  uint8_t *data_buf,
                                  uint16_t buf_size,
                                  uint32_t *next_addr);


#ifdef __cplusplus
}
#endif

#endif /* INC_BLACK_BOX_APP_BLACKBOX_H_ */
