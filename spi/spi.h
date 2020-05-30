#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include "spi_cfg.h"

#if (   !defined(SPI_ENABLE)       \
     || !defined(SPI_DEF_MODE)     \
     || !defined(SPI_DEF_CLK_MODE) \
     || !defined(SPI_DEF_CLK_DIV)  \
     || !defined(SPI_DEF_DATA_ORDER))
#error "One param or more in spi_cfg.h is not defined"
#endif

#if (SPI_ENABLE < 0 || SPI_ENABLE > 1)
#error "SPI_ENABLE must be 0(DISABLED) or 1(ENABLED)"
#endif

#if (SPI_DEF_MODE < 0 || SPI_DEF_MODE > 1)
#error "SPI_DEF_MODE must be 0(master) or 1(slave)"
#endif

#if (SPI_DEF_MODE == 1 && !defined(SPI_RX_BUFF_SIZE))
#error "SPI_RX_BUFF_SIZE must be defined in slave mode"
#endif

#if (SPI_DEF_MODE == 1 && SPI_RX_BUFF_SIZE <= 0)
#error "SPI_RX_BUFF_SIZE must be >= 0"
#endif

#if (SPI_DEF_CLK_MODE < 0 || SPI_DEF_CLK_MODE > 4)
#error "SPI_DEF_CLK_MODE must be between 0 and 4"
#endif

#if (   !(SPI_DEF_CLK_DIV == 2 || SPI_DEF_CLK_DIV == 4    \
     ||  SPI_DEF_CLK_DIV == 8 || SPI_DEF_CLK_DIV == 16    \
     ||  SPI_DEF_CLK_DIV == 32 || SPI_DEF_CLK_DIV == 64   \
     ||  SPI_DEF_CLK_DIV == 128))
#error "SPI_CLK_DIV must be 2, 4, 8, 16, 32, 64 or 128"
#endif

#if (SPI_DEF_DATA_ORDER < 0 || SPI_DEF_DATA_ORDER > 1)
#error "SPI_DEF_DATA_ORDER must be 0(LSB first) or 1(MSB first)"
#endif

typedef enum
{
     SPI_ERR_OK = 0,
     SPI_ERR_NOK,
     SPI_ERR_OPEN,
     SPI_ERR_CLOSE
}SPI_ERR_E;

typedef enum
{
     SPI_MASTER_MODE = 0,
     SPI_SLAVE_MODE = 1
}SPI_MODE_E;

typedef enum
{
     SPI_CLK_MODE_0 = 0,
     SPI_CLK_MODE_1,
     SPI_CLK_MODE_2,
     SPI_CLK_MODE_3
}SPI_CLK_MODE_E;

typedef enum
{
     SPI_CLK_DIV_2 = 2,
     SPI_CLK_DIV_4 = 4,
     SPI_CLK_DIV_8 = 8,
     SPI_CLK_DIV_16 = 16,
     SPI_CLK_DIV_32 = 32,
     SPI_CLK_DIV_64 = 64,
     SPI_CLK_DIV_128 = 128
}SPI_CLK_DIV_E;

typedef enum
{
     SPI_LSB_FIRST = 0,
     SPI_MSB_FIRST
}SPI_DATA_ORDER_E;

typedef enum
{
     SPI_OPEN = 0,
     SPI_CLOSE
}SPI_PERIPH_STATE_E;

void spi_init(void);

SPI_PERIPH_STATE_E spi_getstate(void);

SPI_ERR_E spi_open(void);

SPI_ERR_E spi_close(void);

SPI_ERR_E spi_write(uint16_t ss_id, const void* data,
                    uint16_t nb_bytes);

SPI_ERR_E spi_read(uint16_t ss_id, void* data, 
                   uint16_t nb_bytes, void* dummy);

SPI_ERR_E spi_exchange(uint16_t ss_id, void* read_data,
                       const void* write_data, 
                       uint16_t nb_bytes);
                       
#endif /* __SPI_H__ */