#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include <string.h>

#include "uart_device.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(uart_device, LOG_LEVEL_DBG);


#define UART0_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
#define UART1_DEVICE_NODE DT_CHOSEN(zephyr_modbus)


#define MSG_SIZE 32
 
static int print_uart(const struct device *dev, uint8_t *datas, uint32_t len, int timeout)
{
	for (int i = 0; i < len; i++) {
		uart_poll_out(dev, datas[i]);
	}
    return 0;
}

int uart_init( struct UART_Device *pDev, int baud, char parity, int data_bit, int stop_bit)
{
    const struct device *huart = pDev->priv_data;
	if (!device_is_ready(huart)) {
		printk("UART device not found!");
		return 0;
	}
   
    return 0;
}

int uart_send( struct UART_Device *pDev, uint8_t *datas, uint32_t len, int timeout)
{
    const struct device *huart = pDev->priv_data;
	
    if (0 == print_uart(huart, datas, len, timeout)){
		k_msleep(10); //(minimal is 10 ms, when it belows couldn't get right data in a round)
		 return 0;
	}else{
        return -1;
	}
}


int uart_recv( struct UART_Device *pDev, uint8_t *data, int timeout)
{	
	const struct device *huart = pDev->priv_data;

	if(uart_fifo_read(huart, data, 1) == 1){
		return 0;
	}else{
		return -1;
	}
}

int uart_flush(struct UART_Device *pDev)
{
	uint8_t c;
	const struct device *huart = pDev->priv_data;
	while(uart_fifo_read(huart, &c, 1) == 1){
		LOG_DBG("c:%d", c);
	}
	return 0;
}


static struct UART_Device g_uart0_dev = {"uart0", uart_init, uart_send, uart_recv, uart_flush, DEVICE_DT_GET(UART0_DEVICE_NODE)};
static struct UART_Device g_uart1_dev = {"uart1", uart_init, uart_send, uart_recv, uart_flush, DEVICE_DT_GET(UART1_DEVICE_NODE)};


static struct UART_Device *g_uart_devices[] = {&g_uart0_dev, &g_uart1_dev};


struct UART_Device *GetUARTDevice(char *name)
{
	int i = 0;
	for (i = 0; i < sizeof(g_uart_devices)/sizeof(g_uart_devices[0]); i++)
	{
		if (!strcmp(name, g_uart_devices[i]->name))
			return g_uart_devices[i];
	}
	
	return NULL;
}
