/*
 * app_w25q128.c
 *
 *  Created on: Mar 29, 2026
 *      Author: Asus
 */
#include "Black_box/w25q128.h"

static W25_Handle_t dev;

static W25_Status_t W25_ReadJEDEC_ID(uint8_t id[3]);

static void W25_CS_Low(void)
{
    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_RESET);
}

static void W25_CS_High(void)
{
    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_SET);
}

static W25_Status_t W25_Tx(const uint8_t *data, uint16_t len)
{
    W25_Status_t ret = W25_OK;
    if (HAL_SPI_Transmit(dev.hspi, (uint8_t *)data, len, HAL_MAX_DELAY) != HAL_OK)
        ret =  W25_ERR;
    return ret;
}

static W25_Status_t W25_Rx(uint8_t *data, uint16_t len)
{
    W25_Status_t ret = W25_OK;
    if (HAL_SPI_Receive(dev.hspi, data, len, HAL_MAX_DELAY) != HAL_OK)
        ret = W25_ERR;
    return ret;
}

static W25_Status_t W25_TxRx(const uint8_t *tx, uint8_t *rx, uint16_t len)
{
    W25_Status_t ret = W25_OK;
    if (HAL_SPI_TransmitReceive(dev.hspi, (uint8_t *)tx, rx, len, HAL_MAX_DELAY) != HAL_OK)
        ret = W25_ERR;
    return ret;
}

W25_Status_t W25_Init(void)
{
    uint8_t id[3] = {0};
    W25_Status_t ret = W25_OK;

    dev.hspi = &W25Q128_SPI;
    dev.cs_port = W25_Q128_NCS_PORT;
    dev.cs_pin = W25_Q128_NCS_PIN;

    dev.page_size = 256UL;
    dev.sector_size = 4096UL;
    dev.block32_size = 32768UL;
    dev.block64_size = 65536UL;
    dev.capacity_bytes = 16UL * 1024UL * 1024UL;
    dev.initialized = 0;

    if (dev.hspi == NULL)
    {
        ret = W25_INVALID_PARAM;
    }else
    {
        W25_CS_High();
        HAL_Delay(1);

        if(W25_ReadJEDEC_ID(id) != W25_OK)
        {
            ret = W25_ERR;

        }else if(id[0] == 0xEF && id[2] == 0x18)  /* Winbond W25Q128 usually = EF 40 18 */
        {
            dev.initialized = 1;
            ret = W25_OK;
        }
    }
    return ret;
}

static W25_Status_t W25_ReadJEDEC_ID(uint8_t id[3])
{
    uint8_t tx[4] = {W25_CMD_JEDEC_ID, 0x00, 0x00, 0x00};
    uint8_t rx[4] = {0};
    W25_Status_t ret = W25_OK;

    if (id == NULL)
    {
        ret =  W25_INVALID_PARAM;
    }else
    {
        W25_CS_Low();
        if (W25_TxRx(tx, rx, 4) != W25_OK)
        {
            ret = W25_ERR;
        }
        id[0] = rx[1];
        id[1] = rx[2];
        id[2] = rx[3];
    }
	W25_CS_High();
    return ret;
}

uint8_t W25_ReadStatus1(void)
{
    uint8_t tx[2] = {W25_CMD_READ_SR1, 0x00};
    uint8_t rx[2] = {0};
    uint8_t ret;

    W25_CS_Low();
    if (W25_TxRx(tx, rx, 2) != W25_OK)
    {
        ret = 0xFF;
    }else{
        ret = rx[1];
    }
	W25_CS_High();
    return ret;
}

uint8_t W25_ReadStatus2(void)
{
    uint8_t tx[2] = {W25_CMD_READ_SR2, 0x00};
    uint8_t rx[2] = {0};
    uint8_t ret;

    W25_CS_Low();
    if (W25_TxRx(tx, rx, 2) != W25_OK)
    {
        ret = 0xFF;
    }else{
        ret = rx[1];
    }
	W25_CS_High();
    return ret;
}

uint8_t W25_ReadStatus3(void)
{
    uint8_t tx[2] = {W25_CMD_READ_SR3, 0x00};
    uint8_t rx[2] = {0};
    uint8_t ret;

    W25_CS_Low();
    if (W25_TxRx(tx, rx, 2) != W25_OK)
    {
        ret = 0xFF;
    }else{
        ret = rx[1];
    }
	W25_CS_High();
    return ret;
}

W25_Status_t W25_WriteEnable()
{
    uint8_t cmd = W25_CMD_WRITE_ENABLE;
    W25_Status_t ret = W25_OK;

    W25_CS_Low();
    if (W25_Tx(&cmd, 1) != W25_OK)
    {
        ret = W25_ERR;
    }else
    {
    	W25_CS_High();
        if ((W25_ReadStatus1() & W25_SR1_WEL) == 0)
            ret = W25_ERR;
    }
	W25_CS_High();
    return ret;
}

W25_Status_t W25_WaitBusy(uint32_t timeout_ms)
{
    uint32_t tickstart = HAL_GetTick();
    W25_Status_t ret = W25_OK;

    while (W25_ReadStatus1() & W25_SR1_BUSY)
    {
        if ((HAL_GetTick() - tickstart) > timeout_ms)
        {
            ret = W25_TIMEOUT;
            break;
        }
    }
    return ret;
}

W25_Status_t W25_ChipErase(void)
{
    uint8_t cmd = W25_CMD_CHIP_ERASE;
    W25_Status_t ret = W25_OK;

    if (W25_WriteEnable() != W25_OK)
    {
        ret = W25_ERR;
    }else
    {
		W25_CS_Low();
		if (W25_Tx(&cmd, 1) != W25_OK)
		{
			ret = W25_ERR;
		}
    }
	W25_CS_High();
	ret = W25_WaitBusy(12000);
    return ret;
}

W25_Status_t W25_Read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t cmd[4];
    W25_Status_t ret = W25_OK;

    if (buf == NULL || len == 0)
    {
        ret = W25_INVALID_PARAM;
    }else if ((addr + len) > dev.capacity_bytes)
    {
        ret = W25_INVALID_PARAM;
    }else
    {
        cmd[0] = W25_CMD_READ_DATA;
        cmd[1] = (uint8_t)(addr >> 16);
        cmd[2] = (uint8_t)(addr >> 8);
        cmd[3] = (uint8_t)(addr);

        W25_CS_Low();
        if (W25_Tx(cmd, 4) != W25_OK)
        {
            ret = W25_ERR;
        }else if (W25_Rx(buf, (uint16_t)len) != W25_OK)
        {
            ret =  W25_ERR;
        }
    }
    W25_CS_High();

    return ret;
}

W25_Status_t W25_PageProgram(uint32_t addr, const uint8_t *data, uint16_t len)
{
    uint8_t cmd[4];
    W25_Status_t ret = W25_OK;

    if (data == NULL || len == 0)
    {
        ret = W25_INVALID_PARAM;
    }else if ((addr + len) > dev.capacity_bytes)
    {
    	ret = W25_INVALID_PARAM;
	}else if (len > dev.page_size)
	{
        ret = W25_INVALID_PARAM;
	}else if (((addr & 0xFF) + len) > dev.page_size)/* must not cross page boundary */
	{
        ret = W25_INVALID_PARAM;
	}else if (W25_WriteEnable() != W25_OK)
	{
        ret = W25_ERR;
	}else
	{
	    cmd[0] = W25_CMD_PAGE_PROGRAM;
	    cmd[1] = (uint8_t)(addr >> 16);
	    cmd[2] = (uint8_t)(addr >> 8);
	    cmd[3] = (uint8_t)(addr);

	    W25_CS_Low();
	    if (W25_Tx( cmd, 4) != W25_OK)
	    {
	        ret = W25_ERR;
	    }else if (W25_Tx(data, len) != W25_OK)
	    {
	        ret = W25_ERR;
	    }

	    W25_CS_High();
	    ret = W25_WaitBusy(20);

	}
    return ret;
}

W25_Status_t W25_Write(uint32_t addr, const uint8_t *data, uint32_t len)
{
    uint32_t remaining;
    uint16_t chunk;
    uint32_t page_offset;
    W25_Status_t ret = W25_OK;

    if (data == NULL || len == 0)
    {
        ret = W25_INVALID_PARAM;
    }else if ((addr + len) > dev.capacity_bytes)
    {
        ret = W25_INVALID_PARAM;
    }else
    {
        remaining = len;

        while (remaining > 0)
        {
            page_offset = addr % dev.page_size;
            chunk = (uint16_t)(dev.page_size - page_offset);

            if (chunk > remaining)
            {
                chunk = (uint16_t)remaining;
            }
            if (W25_PageProgram(addr, data, chunk) != W25_OK)
            {
                ret = W25_ERR;
                break;
            }

            addr += chunk;
            data += chunk;
            remaining -= chunk;
        }
    }

    return ret;
}

W25_Status_t W25_ReadByte(uint32_t addr, uint8_t *data)
{
    return W25_Read(addr, data, 1);
}

W25_Status_t W25_WriteByte(uint32_t addr, uint8_t data)
{
    return W25_PageProgram(addr, &data, 1);
}


W25_Status_t W25_FindFirstEmpty(uint32_t start_addr, uint32_t end_addr, uint32_t *empty_addr)
{
    uint8_t b = 0x00;
    uint32_t addr;
    W25_Status_t ret = W25_OK;

    if (empty_addr == NULL)
    {
        ret = W25_INVALID_PARAM;
    }else if (start_addr >= end_addr || end_addr > dev.capacity_bytes)
    {
        ret = W25_INVALID_PARAM;
    }else
    {
        for (addr = start_addr; addr < end_addr; addr++)
        {
            if (W25_ReadByte(addr, &b) != W25_OK)
            {
            	ret = W25_ERR;
            	break;
            }else if (b == 0xFF)
            {
                *empty_addr = addr;
                ret = W25_OK;
            }
        }
    }

    return ret;
}

W25_Status_t W25_AppendRecord(uint32_t *write_addr, uint32_t end_addr, const uint8_t *data, uint16_t len)
{
    uint8_t header[2];
    uint32_t addr;
    W25_Status_t ret = W25_OK;

    if (write_addr == NULL || data == NULL || len == 0)
    {
    	ret = W25_INVALID_PARAM;
    }else
    {
		addr = *write_addr;

		if ((addr + 2UL + len) > end_addr)
		{
			ret = W25_INVALID_PARAM;
		}else
		{
			header[0] = (uint8_t)(len & 0xFF);
			header[1] = (uint8_t)((len >> 8) & 0xFF);

			if (W25_Write(addr, header, 2) != W25_OK)
			{
				ret =  W25_ERR;
			}else{
				addr += 2;

				if (W25_Write(addr, data, len) != W25_OK)
				{
					ret = W25_ERR;
				}else
				{
				addr += len;
				*write_addr = addr;
				}
			}
		}
    }
    return ret;
}

