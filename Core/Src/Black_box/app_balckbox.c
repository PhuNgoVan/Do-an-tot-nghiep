/*
 * app_balckbox.c
 *
 *  Created on: Mar 29, 2026
 *      Author: Asus
 */

#ifndef SRC_BLACK_BOX_APP_BALCKBOX_C_
#define SRC_BLACK_BOX_APP_BALCKBOX_C_

#include "Black_box/app_blackbox.h"
#include <string.h>

typedef struct __attribute__((packed))
{
    uint32_t magic;
    uint32_t seq;
    uint32_t tick_ms;
    uint8_t  type;
    uint16_t data_len;
    uint16_t crc;
} APP_BB_PayloadHeader_t;

static uint16_t APP_BB_CalcCRC(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0;
    uint16_t i = 0;

    if (data != 0)
    {
        for (i = 0; i < len; i++)
        {
            crc ^= data[i];
        }
    }

    return crc;
}
/*Check blackbox ic is available??
Check find emty space from start to -> ???*/
APP_BB_Status_t APP_BB_Init(APP_BB_Handle_t *bb, uint32_t start_addr, uint32_t end_addr)
{
    APP_BB_Status_t ret = APP_BB_OK;
    W25_Status_t wret = W25_OK;
    uint32_t empty_addr = 0;

    if (bb == 0)
    {
        ret = APP_BB_INVALID_PARAM;
    }
    else if ((start_addr >= end_addr) || (end_addr > (16UL * 1024UL * 1024UL)))
    {
        ret = APP_BB_INVALID_PARAM;
    }
    else
    {
        memset(bb, 0, sizeof(APP_BB_Handle_t));

        wret = W25_Init();
        if (wret != W25_OK)
        {
            ret = APP_BB_ERR;
        }
        else
        {
            bb->start_addr = start_addr;
            bb->end_addr   = end_addr;
            bb->seq        = 0;

            wret = W25_FindFirstEmpty(start_addr, end_addr, &empty_addr);
            if (wret == W25_OK)
            {
                bb->write_addr = empty_addr;
            }
            else
            {
                bb->write_addr = start_addr;
            }

            bb->initialized = 1;
            ret = APP_BB_OK;
        }
    }

    return ret;
}

//Log data from start to start addr + len + 0x14
APP_BB_Status_t APP_BB_Log(APP_BB_Handle_t *bb, uint8_t type, const uint8_t *data, uint16_t len)
{
    APP_BB_Status_t ret = APP_BB_OK;
    W25_Status_t wret = W25_OK;

    APP_BB_PayloadHeader_t hdr;
    uint8_t payload[sizeof(APP_BB_PayloadHeader_t) + APP_BB_MAX_USER_DATA];
    uint16_t payload_len = 0;
    uint32_t remain = 0;

    if (bb == 0)
    {
        ret = APP_BB_INVALID_PARAM;
    }
    else if (bb->initialized == 0)
    {
        ret = APP_BB_NOT_INIT;
    }
    else if ((data == 0) && (len > 0))
    {
        ret = APP_BB_INVALID_PARAM;
    }
    else if (len > APP_BB_MAX_USER_DATA)
    {
        ret = APP_BB_INVALID_PARAM;
    }
    else
    {
        hdr.magic    = APP_BB_MAGIC;
        hdr.seq      = bb->seq;
        hdr.tick_ms  = HAL_GetTick();
        hdr.type     = type;
        hdr.data_len = len;
        hdr.crc      = APP_BB_CalcCRC(data, len);

        memcpy(payload, &hdr, sizeof(APP_BB_PayloadHeader_t));

        if (len > 0)
        {
            memcpy(&payload[sizeof(APP_BB_PayloadHeader_t)], data, len);
        }

        payload_len = sizeof(APP_BB_PayloadHeader_t) + len;

        remain = bb->write_addr - bb->start_addr + 1;
        if ((payload_len + 2U) > remain)
        {
            ret = APP_BB_FULL;
        }
        else
        {
            wret = W25_AppendRecord(&bb->start_addr, bb->end_addr, payload, payload_len);
            if (wret != W25_OK)
            {
                ret = APP_BB_ERR;
            }
            else
            {
                bb->seq++;
            }
        }
    }

    return ret;
}

/*Read data from start byte each chunk and auto decode data into buffer*/
APP_BB_Status_t APP_BB_ReadRecord(APP_BB_Handle_t *bb,
                                  uint32_t addr,
                                  APP_BB_RecordInfo_t *info,
                                  uint8_t *data_buf,
                                  uint16_t buf_size,
                                  uint32_t *next_addr)
{
    APP_BB_Status_t ret = APP_BB_OK;
    W25_Status_t wret = W25_OK;

    uint8_t len_raw[2] = {0};
    uint16_t rec_len = 0;
    APP_BB_PayloadHeader_t hdr;

    if ((bb == 0) || (info == 0) || (next_addr == 0))
    {
        ret = APP_BB_INVALID_PARAM;
    }
    else if (bb->initialized == 0)
    {
        ret = APP_BB_NOT_INIT;
    }
    else if ((addr < bb->start_addr) || (addr >= bb->end_addr))
    {
        ret = APP_BB_INVALID_PARAM;
    }
    else
    {
        wret = W25_Read(addr, len_raw, 2);
        if (wret != W25_OK)
        {
            ret = APP_BB_ERR;
        }
        else
        {
            rec_len = (uint16_t)((uint16_t)len_raw[0] | ((uint16_t)len_raw[1] << 8));

            if ((rec_len == 0xFFFFU) || (rec_len == 0U))
            {
                ret = APP_BB_ERR;
            }
            else if ((addr + 2UL + rec_len) > bb->end_addr)
            {
                ret = APP_BB_ERR;
            }
            else if (rec_len < sizeof(APP_BB_PayloadHeader_t))
            {
                ret = APP_BB_ERR;
            }
            else
            {
                wret = W25_Read(addr + 2U, (uint8_t *)&hdr, sizeof(APP_BB_PayloadHeader_t));
                if (wret != W25_OK)
                {
                    ret = APP_BB_ERR;
                }
                else if (hdr.magic != APP_BB_MAGIC)
                {
                    ret = APP_BB_ERR;
                }
                else if (hdr.data_len > APP_BB_MAX_USER_DATA)
                {
                    ret = APP_BB_ERR;
                }
                else if (hdr.data_len > (rec_len - sizeof(APP_BB_PayloadHeader_t)))
                {
                    ret = APP_BB_ERR;
                }
                else if ((hdr.data_len > 0U) && ((data_buf == 0) || (buf_size < hdr.data_len)))
                {
                    ret = APP_BB_INVALID_PARAM;
                }
                else
                {
                    info->magic    = hdr.magic;
                    info->seq      = hdr.seq;
                    info->tick_ms  = hdr.tick_ms;
                    info->type     = hdr.type;
                    info->data_len = hdr.data_len;
                    info->crc      = hdr.crc;

                    if (hdr.data_len > 0U)
                    {
                        wret = W25_Read(addr + 2U + sizeof(APP_BB_PayloadHeader_t),
                                        data_buf,
                                        hdr.data_len);
                        if (wret != W25_OK)
                        {
                            ret = APP_BB_ERR;
                        }
                        else if (APP_BB_CalcCRC(data_buf, hdr.data_len) != hdr.crc)
                        {
                            ret = APP_BB_ERR;
                        }
                    }

                    if (ret == APP_BB_OK)
                    {
                        *next_addr = addr + 2UL + rec_len;
                    }
                }
            }
        }
    }

    return ret;
}

#endif /* SRC_BLACK_BOX_APP_BALCKBOX_C_ */
