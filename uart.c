#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "ringbuffer.h"

#define UART_PERIPH_CONFIG()		(uart_periph.uart_config)
#define UART_PERIPH_CLOSED()		(uart_periph.status == PERIPH_CLOSE)
#define UART_PERIPH_OPENED()		(uart_periph.status == PERIPH_OPEN)

#define BAUDRATE_SCALLER(baud,div)	((F_CPU /(div*baud)) - 1)
#define NORMAL_MODE_DIV				16UL
#define DS_MODE_DIV					8UL

static void uart_set_baudrate(UART_BAUDRATE_E baudrate, 
							uint32_t dividor);
static void uart_enable_tx(void);
static void uart_enable_rx(void);
static void uart_disable_tx(void);
static void uart_disable_rx(void);
static void uart_set_frame_format(UART_PARITY_E uart_parity,
								  UART_STOP_BITS_E uart_stop_bits,
								  UART_NB_BITS_E uart_nb_bits);

typedef enum
{
	PERIPH_CLOSE = 0,
	PERIPH_OPEN
}UART_PERIPH_STATUS_E;

typedef struct
{
#if (UART_TX_EN == 1)
	ring_buffer_s tx_ring;
#endif
#if (UART_RX_EN == 1)
	ring_buffer_s rx_ring;
#endif
	uart_config_s uart_config;
	UART_PERIPH_STATUS_E status;
}uart_periph_s;

uart_periph_s uart_periph = {0};
#if (UART_TX_EN == 1)
uint8_t tx_buffer[UART_TX_BUFF_SIZE] = {0};
#endif
#if (UART_RX_EN == 1)
uint8_t rx_buffer[UART_RX_BUFF_SIZE] = {0};
#endif

#if (UART_RX_EN == 1)
ISR(USART_RX_vect)
{
	uint8_t data = UDR0;

	if(!(UCSR0A & ((1<<FE0)|(1<<DOR0)|(1<<UPE0))))
	{
		ringbuffer_queuebyte(&uart_periph.rx_ring,&data);
	}
}
#endif

#if (UART_TX_EN == 1)
ISR(USART_UDRE_vect)
{
    uint8_t data;

    if(RING_BUFFER_EMPTY((&uart_periph.tx_ring)))
    {
        UCSR0B &= ~(1 << UDRIE0);
    }
    else
	{
    	ringbuffer_dequeuebyte(&uart_periph.tx_ring , &data);
		UDR0 = data;
	}
}
#endif

static void uart_set_baudrate(UART_BAUDRATE_E baudrate, 
							uint32_t dividor)
{
	uint16_t baud_scaller = BAUDRATE_SCALLER(baudrate,dividor);

	UBRR0L = (uint8_t)(baud_scaller);
	UBRR0H = (uint8_t)(baud_scaller >> 8);
}

static void uart_enable_tx(void)
{
#if (UART_TX_EN == 1)
	UCSR0B |= (1 << TXEN0);
#endif
}

static void uart_enable_rx(void)
{
#if (UART_RX_EN == 1)
	UCSR0B |= (1 << RXCIE0)|(1 << RXEN0);
#endif
}

static void uart_disable_tx(void)
{
#if (UART_TX_EN == 1)
	UCSR0B &= ~(1 << TXEN0);
#endif
}

static void uart_disable_rx(void)
{
#if (UART_RX_EN == 1)
	UCSR0B &= ~((1 << RXCIE0)|(1 << RXEN0));
#endif
}

static void uart_set_frame_format(UART_PARITY_E uart_parity,
								  UART_STOP_BITS_E uart_stop_bits,
								  UART_NB_BITS_E uart_nb_bits)
{
	switch(uart_parity)
	{
		case UART_PARITY_ODD:
			UCSR0C = 3<<UPM00;
		break;

		case UART_PARITY_EVEN:
			UCSR0C = 2<<UPM00;
		break;

		case UART_PARITY_NONE:
			UCSR0C = 0;
		break;

		default:
		break;
	}

	switch(uart_nb_bits)
	{
		case UART_NB_BITS_6:
			UCSR0C |= 1<<UCSZ00;
		break;

		case UART_NB_BITS_7:
			UCSR0C |= 2<<UCSZ00;
		break;
		case UART_NB_BITS_8:
			UCSR0C |= 3<<UCSZ00;
		break;
		case UART_NB_BITS_9:
			UCSR0C |= 7<<UCSZ00;
		break;

		default:
		break;
	}
	
	if(uart_stop_bits == UART_STOP_BITS_2)
	{
		UCSR0C |= 1<<USBS0;
	}
}

void uart_init(void)
{
#if (UART_TX_EN == 1)
	ringbuffer_init(&uart_periph.tx_ring, tx_buffer, UART_TX_BUFF_SIZE);
#endif
#if (UART_RX_EN == 1)
	ringbuffer_init(&uart_periph.rx_ring, rx_buffer, UART_RX_BUFF_SIZE);
#endif
	uart_periph.status = PERIPH_CLOSE;
}

UART_ERR_E uart_open_default(void)
{	
	if(UART_PERIPH_CLOSED())
	{
		uart_periph.status = PERIPH_OPEN;
#if (UART_CONFIG_DOUBLE_SPEED)
		uart_set_baudrate(UART_CONFIG_BAUDRATE,
						DS_MODE_DIV);
#else
		uart_set_baudrate(UART_CONFIG_BAUDRATE,
						NORMAL_MODE_DIV);
#endif
		uart_set_frame_format(UART_CONFIG_PARITY,
							UART_CONFIG_STOP_BITS,
							UART_CONFIG_NB_BITS);
		uart_enable_tx();
		uart_enable_rx();

		return UART_ERR_OK;
	}
	else
	{
		return UART_ERR_OPEN;
	}
}

UART_ERR_E uart_open(uart_config_s* uart_config)
{
	if(UART_PERIPH_CLOSED())
	{
		uart_periph.status = PERIPH_OPEN;
		uart_set_baudrate(uart_config->uart_baudrate,
		(uart_config->uart_en_double_speed ? DS_MODE_DIV : NORMAL_MODE_DIV));
		uart_set_frame_format(uart_config->uart_parity,
							uart_config->uart_stop_bits,
							uart_config->uart_nb_bits);
		uart_enable_tx();
		uart_enable_rx();
		
		return UART_ERR_OK;
	}	
	else
	{
		return UART_ERR_OPEN;
	}

}

UART_ERR_E uart_close(void)
{
	if(UART_PERIPH_OPENED())
	{
		uart_periph.status = PERIPH_CLOSE;
		uart_disable_rx();
		uart_disable_tx();

		return UART_ERR_OK;
	}
	else
	{
		return UART_ERR_CLOSE;
	}
}

UART_ERR_E uart_readbyte(void *byte)
{
#if (UART_RX_EN == 1)
	if(byte)
	{
		if(UART_PERIPH_OPENED())
		{
			if(ringbuffer_dequeuebyte(&uart_periph.rx_ring, 
			   byte) == RINGBUFF_OK)
			{
				return UART_ERR_OK;
			}
			else
			{
				return UART_ERR_NOK;
			}
		}
		else
		{
			return UART_ERR_CLOSE;
		}
	}
#endif
	return UART_ERR_NOK;
}

UART_ERR_E uart_readbuff(void *buff, uint16_t nb_bytes)
{
#if (UART_RX_EN == 1)
	if(buff && nb_bytes)
	{
		if(UART_PERIPH_OPENED())
		{
			if(ringbuffer_dequeuebuff(&uart_periph.rx_ring,buff,
			   nb_bytes) == RINGBUFF_OK)
			{
				return UART_ERR_OK;
			}
			else
			{
				return UART_ERR_NOK;
			}
		}
		else
		{
			return UART_ERR_CLOSE;
		}
	}
#endif
	return UART_ERR_NOK;
}

UART_ERR_E uart_writebyte(const void *byte)
{
#if (UART_TX_EN == 1)
	if(byte)
	{
		if(UART_PERIPH_OPENED())
		{
			if(ringbuffer_queuebyte(&uart_periph.tx_ring,
			   byte) == RINGBUFF_OK)
			{
				UCSR0B |= (1 << UDRIE0);

				return UART_ERR_OK;
			}
			else
			{
				return UART_ERR_NOK;
			}
		}
		else
		{
			return UART_ERR_CLOSE;
		}
	}
#endif
	return UART_ERR_NOK;	
}

UART_ERR_E uart_writebuff(void *buff, uint16_t nb_bytes)
{
#if (UART_TX_EN == 1)
	if(buff && nb_bytes)
	{
		if(UART_PERIPH_OPENED())
		{
			if(ringbuffer_queuebuff(&uart_periph.tx_ring,buff,
			   nb_bytes) == RINGBUFF_OK)
			{
				UCSR0B |= (1 << UDRIE0);

				return UART_ERR_OK;
			}
			else
			{
				return UART_ERR_NOK;
			}
		}
		else
		{
			return UART_ERR_CLOSE;
		}
	}
#endif
	return UART_ERR_NOK;
}
