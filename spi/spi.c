#include "spi.h"
#include "ringbuffer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

SPI_PERIPH_STATE_E Spi_State = SPI_CLOSE;

#if (SPI_DEF_MODE == 1)
uint8_t spi_rx_buff[SPI_RX_BUFF_SIZE] = {0};
ring_buffer_s spi_rx_ring_buff = {0};

ISR(SPI_STC_vect)
{
    uint8_t data = SPDR;
    if(!(SPSR & (1 << WCOL)))
    {
        ringbuffer_queuebyte(spi_rx_ring_buff, data);
    }
}
#endif

void spi_init(void)
{
#if (SPI_ENABLE == 1)
    Spi_State = SPI_CLOSE;
    spi_gpio_init();
#elif (SPI_DEF_MODE == 1)
    ringbuffer_init(spi_rx_ring_buff, spi_rx_buff, 
                   SPI_RX_BUFF_SIZE);
#endif
}

SPI_PERIPH_STATE_E spi_getstate(void)
{
    return Spi_State;
}

SPI_ERR_E spi_open(void)
{
#if (SPI_ENABLE == 1)
    uint16_t spcr_reg = 0;
    uint16_t spsr_reg = 0;

    if(Spi_State == SPI_OPEN)
    {
        return SPI_ERR_OPEN;
    }
    else
    {
        /* Init periph */
        /* Data Order */
        spcr_reg |= (SPI_DEF_DATA_ORDER << DORD);

        /* Spi mode */
        switch(SPI_DEF_CLK_MODE)
        {
            case 0:                             break;
            case 1: spcr_reg |= (1 << CPHA);    break;      
            case 2: spcr_reg |= (1 << CPOL);    break;
            case 3: spcr_reg |= (3 << CPHA);    break;
        }

        /* Spi clk */
        switch(SPI_DEF_CLK_DIV)
        {
            case 2: spsr_reg |= (1 << SPI2X);   break;
            case 4:                             break;
            case 8:
                spsr_reg |= (1 << SPI2X);
                spcr_reg |= (1 << SPR0);
            break;
            case 16: spcr_reg |= (1 << SPR0);   break;
            case 32:
                spsr_reg |= (1 << SPI2X);
                spcr_reg |= (1 << SPR1);
            break;
            case 64: spcr_reg |= (1 << SPR1);   break;
            case 128: spcr_reg |= (3 << SPR0);  break;
        }

        /* Spi Enable */
        spcr_reg |= (1 << SPE);
#if (SPI_DEF_MODE == 0)
        /* Master mode */
        spcr_reg |= (1 << MSTR);
#elif (SPI_DEF_MODE == 1)
        spcr_reg |= (1 << SPIE);
#endif
        SPCR = spcr_reg;
        SPSR = spsr_reg;
        Spi_State = SPI_OPEN;

        return SPI_ERR_OK;
    }

#endif

return SPI_ERR_NOK;
}

SPI_ERR_E spi_close(void)
{
#if (SPI_ENABLE == 1)
    if(Spi_State == SPI_CLOSE)
    {
        return SPI_ERR_CLOSE;
    }
    else
    {
        SPCR = 0x00;
        Spi_State = SPI_CLOSE;

        return SPI_ERR_OK;
    }
#endif
    return SPI_ERR_NOK;
}

SPI_ERR_E spi_write(uint16_t ss_id, const void* data,
                    uint16_t nb_bytes)
{
#if (SPI_ENABLE == 1 && SPI_DEF_MODE == 0)
    const uint8_t* data_u8 = (const uint8_t*)data;
    uint8_t rd_data = 0;

    if(data_u8 && nb_bytes)
    {
        if(Spi_State == SPI_OPEN)
        {
            spi_ss_set_low(ss_id);
            while(nb_bytes--)
            {
                SPDR = *data_u8++;
                while(!(SPSR & (1 << SPIF)));
                rd_data = SPDR;
                (void)rd_data;
            }
            spi_ss_set_high(ss_id);

            return SPI_ERR_OK;
        }
        else
        {
            return SPI_ERR_CLOSE;
        }
    }
#endif
    return SPI_ERR_NOK;
}

SPI_ERR_E spi_read(uint16_t ss_id, void* data, 
                   uint16_t nb_bytes, void* dummy)
{
#if (SPI_ENABLE == 1)
    uint8_t* data_u8 = (uint8_t*)data;

    if(data_u8 && data)
    {
        if(Spi_State == SPI_OPEN)
        {
#if (SPI_DEF_MODE == 0)
            spi_ss_set_low(ss_id);
            while(nb_bytes--)
            {
                SPDR = *((uint8_t*)dummy);
                while(!(SPSR & (1 << SPIF)));
                *data_u8++ = SPDR;
            }
            spi_ss_set_high(ss_id);
#elif (SPI_DEF_MODE == 1)
            if(!RING_BUFFER_EMPTY(spi_rx_ring_buff))
            {
                ringbuffer_dequeuebuff(spi_rx_ring_buff,
                                      data_u8, nb_bytes);
            }
#endif
            return SPI_ERR_OK;
        }
        else
        {
            return SPI_ERR_CLOSE;
        }
    }
#endif
    return SPI_ERR_NOK;
}

SPI_ERR_E spi_exchange(uint16_t ss_id, void* read_data,
                       const void* write_data, 
                       uint16_t nb_bytes)
{
#if (SPI_ENABLE == 1 && SPI_DEF_MODE == 0)
    uint8_t* rd_data_u8 = (uint8_t*)read_data;
    const uint8_t* wr_data_u8 = (const uint8_t*)write_data;

    if(rd_data_u8 && wr_data_u8)
    {
        spi_ss_set_low(ss_id);
        while(nb_bytes--)
        {
            SPDR = *wr_data_u8++;
            while(!(SPSR & (1 << SPIF)));
            *rd_data_u8++ = SPDR;
        }
        spi_ss_set_high(ss_id);

        return SPI_ERR_OK;
    }
#endif
    return SPI_ERR_NOK;
}
