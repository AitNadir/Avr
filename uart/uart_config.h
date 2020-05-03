#ifndef __UART_CONFIG_H__
#define __UART_CONFIG_H__

#define F_CPU                       8000000

#define UART_TX_EN                  1
#define UART_RX_EN                  1
#define UART_TX_BUFF_SIZE           16
#define UART_RX_BUFF_SIZE           16

#define UART_CONFIG_PARITY          0
#define UART_CONFIG_STOP_BITS       1
#define UART_CONFIG_NB_BITS         8
#define UART_CONFIG_BAUDRATE        9600
#define UART_CONFIG_DOUBLE_SPEED    0

#endif /* uart_config.h */