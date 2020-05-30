#ifndef __SPI_CFG_H__
#define __SPI_CFG_H__

#define SPI_ENABLE              1
#define SPI_DEF_MODE            0
#define SPI_DEF_CLK_MODE        1
#define SPI_DEF_CLK_DIV         16
#define SPI_DEF_DATA_ORDER      0


static inline void spi_gpio_init(void)
{
}

static inline void spi_ss_set_low(uint16_t ss_id)
{
}

static inline void spi_ss_set_high(uint16_t ss_id)
{
}

#endif /* __SPI_CFG_H__ */