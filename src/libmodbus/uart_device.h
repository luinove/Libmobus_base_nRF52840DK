#ifndef __UART_DEVICE_H
#define __UART_DEVICE_H

#include <stdint.h>

typedef struct UART_Device {
    char *name;
	int (*Init)( struct UART_Device *pDev, int baud, char parity, int data_bit, int stop_bit);
	int (*Send)( struct UART_Device *pDev, uint8_t *datas, uint32_t len, int timeout);
	int (*RecvByte)( struct UART_Device *pDev, uint8_t *data, int timeout);
	int (*Flush)(struct UART_Device *pDev);
    const void *priv_data;
}UART_Device, *PUART_Device;

UART_Device *GetUARTDevice(char *name);

    
#endif /* __UART_DEVICE_H */

