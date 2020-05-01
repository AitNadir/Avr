#ifndef __UART_H__
#define __UART_H__

/**
 * Driver for the UART peripheral for serial communication.
 * Files:
 * inc/uart.h
 * inc/uart_config.h
 * src/uart.c
*/

#include "uart_config.h"

#if (!defined(UART_TX_EN) 			||	\
	!defined(UART_RX_EN) 			||	\
	!defined(UART_CONFIG_PARITY) 	||	\
	!defined(UART_CONFIG_STOP_BITS) ||	\
	!defined(UART_CONFIG_NB_BITS)	||	\
	!defined(UART_CONFIG_BAUDRATE)  ||  \
	!defined(UART_CONFIG_DOUBLE_SPEED))
#error "one or more config parameters in uart_config.h are missing"
#endif

#if(UART_TX_EN > 1)
#error "UART_TX_EN must be 0 or 1"
#endif

#if(UART_RX_EN > 1)
#error "UART_RX_EN must be 0 or 1"
#endif

#if(UART_TX_EN == 1 && !defined(UART_TX_BUFF_SIZE))
#error "UART_TX_BUFF_SIZE is missing from uart_config.h"
#endif

#if(UART_RX_EN == 1 && !defined(UART_RX_BUFF_SIZE))
#error "UART_RX_BUFF_SIZE is missing from uart_config.h"
#endif

#if(UART_TX_EN == 1 && UART_TX_BUFF_SIZE == 0)
#error "UART_TX_BUFF_SIZE must be > 0 when TX enabled"
#endif

#if(UART_RX_EN == 1 && UART_RX_BUFF_SIZE == 0)
#error "UART_RX_BUFF_SIZE must be > 0 when RX enabled"
#endif

#if(UART_CONFIG_DOUBLE_SPEED > 1)
#error "UART_CONFIG_DOUBLE_SPEED must be 0 or 1"
#endif

#if(UART_CONFIG_PARITY > 2)
#error "UART_CONFIG_PARITY must be 0, 1 or 2"
#endif

#if(UART_CONFIG_STOP_BITS > 2)
#error "UART_CONFIG_STOP_BITS must be 1 or 2"
#endif

#if(UART_CONFIG_NB_BITS > 9)
#error "UART_CONFIG_NB_BITS must be less than 9"
#endif

#if	!(UART_CONFIG_BAUDRATE == 2400UL ||	\
	UART_CONFIG_BAUDRATE == 4800UL ||	\
	UART_CONFIG_BAUDRATE == 9600UL ||	\
	UART_CONFIG_BAUDRATE == 14400UL ||	\
	UART_CONFIG_BAUDRATE == 19200UL ||	\
	UART_CONFIG_BAUDRATE == 28800UL ||	\
	UART_CONFIG_BAUDRATE == 38400UL ||	\
	UART_CONFIG_BAUDRATE == 57600UL ||	\
	UART_CONFIG_BAUDRATE == 76800UL ||	\
	UART_CONFIG_BAUDRATE == 115200UL ||	\
	UART_CONFIG_BAUDRATE == 230400UL ||	\
	UART_CONFIG_BAUDRATE == 250000UL ||	\
	UART_CONFIG_BAUDRATE == 500000UL ||	\
	UART_CONFIG_BAUDRATE == 1000000UL || \
	UART_CONFIG_BAUDRATE == 2000000UL)
#error "UART_CONFIG_BAUDRATE is not allowed"
#endif

typedef enum
{
	UART_ERR_OK = 0,
	UART_ERR_OPEN,
	UART_ERR_CLOSE,
	UART_ERR_NOK
}UART_ERR_E;

typedef enum
{
	UART_DISABLED = 0,
	UART_ENABLED
}UART_EN_E;

/** Specify the parity */
typedef enum
{
	UART_PARITY_NONE = 0,
	UART_PARITY_ODD,
	UART_PARITY_EVEN
}UART_PARITY_E;

/** Specify the stop bit */
typedef enum
{
	UART_STOP_BITS_1 = 1,
	UART_STOP_BITS_2,
}UART_STOP_BITS_E;

/** Specify number of bits */
typedef enum
{
	UART_NB_BITS_5 = 5,
	UART_NB_BITS_6,
	UART_NB_BITS_7,
	UART_NB_BITS_8,
	UART_NB_BITS_9 = 9
}UART_NB_BITS_E;

/** Specify the baudrate */
typedef enum
{
	UART_BAUDRATE_2400 = 2400UL,
	UART_BAUDRATE_4800 = 4800UL,
	UART_BAUDRATE_9600 = 9600UL,
	UART_BAUDRATE_14K4 = 14400UL,
	UART_BAUDRATE_19K2 = 19200UL,
	UART_BAUDRATE_28K8 = 28800UL,
	UART_BAUDRATE_38K4 = 38400UL,
	UART_BAUDRATE_57K6 = 57600UL,
	UART_BAUDRATE_76K8 = 76800UL,
	UART_BAUDRATE_115K2 = 115200UL,
	UART_BAUDRATE_230K4 = 230400UL,
	UART_BAUDRATE_250K = 250000UL,
	UART_BAUDRATE_500K = 500000UL,
	UART_BAUDRATE_1M = 1000000UL,
	UART_BAUDRATE_2M = 2000000UL
}UART_BAUDRATE_E;

typedef struct
{
	UART_PARITY_E uart_parity;
	UART_STOP_BITS_E uart_stop_bits;
	UART_NB_BITS_E uart_nb_bits;
	UART_BAUDRATE_E uart_baudrate;
	UART_EN_E uart_en_double_speed;
}uart_config_s;

/** Init the uart driver */
void uart_init(void);

/** Open the uart communication with the default parameters */
UART_ERR_E uart_open_default(void);

/** Open the uart communication with defined parameters */
UART_ERR_E uart_open(uart_config_s* uart_config);

/** Close the uart communication */
UART_ERR_E uart_close(void);

/** Read one byte. The uart communication should be open */
UART_ERR_E uart_readbyte(void *byte);

/** Read a buffer of bytes. The uart communication should be open */
UART_ERR_E uart_readbuff(void *buff, uint16_t nb_bytes);

/** Write one byte. The uart communication should be open */
UART_ERR_E uart_writebyte(const void *byte);

/** Write a buffer of bytes. The uart communication should be open */
UART_ERR_E uart_writebuff(void *buff, uint16_t nb_bytes);

#endif	/* uart.h */