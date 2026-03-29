#ifndef __W25Q128_H__
#define __W25Q128_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include "main.h"

// Pin cs
extern SPI_HandleTypeDef hspi1;


#define W25Q128_SPI			hspi1
#define W25_Q128_NCS_PORT	GPIOB
#define	W25_Q128_NCS_PIN	GPIO_PIN_0

typedef enum
{
    W25_OK = 0,
    W25_ERR,
    W25_TIMEOUT,
    W25_INVALID_PARAM
} W25_Status_t;

typedef struct
{
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;

    uint32_t page_size;       // 256
    uint32_t sector_size;     // 4096
    uint32_t block32_size;    // 32768
    uint32_t block64_size;    // 65536
    uint32_t capacity_bytes;  // 16MB = 16777216

    uint8_t initialized;
} W25_Handle_t;

/* Commands */
#define W25_CMD_JEDEC_ID         0x9F
#define W25_CMD_READ_DATA        0x03
#define W25_CMD_FAST_READ        0x0B
#define W25_CMD_PAGE_PROGRAM     0x02
#define W25_CMD_WRITE_ENABLE     0x06
#define W25_CMD_WRITE_DISABLE    0x04
#define W25_CMD_READ_SR1         0x05
#define W25_CMD_READ_SR2         0x35
#define W25_CMD_READ_SR3         0x15
#define W25_CMD_SECTOR_ERASE_4K  0x20
#define W25_CMD_BLOCK_ERASE_32K  0x52
#define W25_CMD_BLOCK_ERASE_64K  0xD8
#define W25_CMD_CHIP_ERASE       0xC7

/* SR1 bits */
#define W25_SR1_BUSY             0x01
#define W25_SR1_WEL              0x02

/* init */
W25_Status_t W25_Init(void);

uint8_t      W25_ReadStatus1(void);
uint8_t      W25_ReadStatus2(void);
uint8_t      W25_ReadStatus3(void);
W25_Status_t W25_WriteEnable(void);
W25_Status_t W25_WaitBusy(uint32_t timeout_ms);

/* erase */
W25_Status_t W25_EraseSector4K(uint32_t addr);
W25_Status_t W25_EraseBlock32K(uint32_t addr);
W25_Status_t W25_EraseBlock64K(uint32_t addr);
W25_Status_t W25_ChipErase(void);

/* read/write */
W25_Status_t W25_Read(uint32_t addr, uint8_t *buf, uint32_t len);
W25_Status_t W25_PageProgram(uint32_t addr, const uint8_t *data, uint16_t len);
W25_Status_t W25_Write(uint32_t addr, const uint8_t *data, uint32_t len);

/* util */
W25_Status_t W25_ReadByte(uint32_t addr, uint8_t *data);
W25_Status_t W25_WriteByte(uint32_t addr, uint8_t data);

/* black box helper */
W25_Status_t W25_FindFirstEmpty(uint32_t start_addr, uint32_t end_addr, uint32_t *empty_addr);
W25_Status_t W25_AppendRecord(uint32_t *write_addr, uint32_t end_addr, const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif

