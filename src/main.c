#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/gpio.h>
// #include <zephyr/modbus/modbus.h>

#include <zephyr/logging/log.h>
#include "libmodbus/modbus.h"
#include "libmodbus/uart_device.h"
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

int main(void)
{
	modbus_t *ctx;
	int rc;
	uint16_t temp[1];

	ctx = modbus_new_st_rtu("uart1", 9600, 'N', 8, 1);
	modbus_set_slave(ctx, 1);

	rc = modbus_connect(ctx);
	if(rc == 1)
	{
		modbus_free(ctx);
	}
	
	rc = modbus_read_registers(ctx, 0x06, 1, temp); 
	LOG_DBG("rc:%d;temp06 :%d", rc,  temp[0]);

	rc = modbus_read_registers(ctx, 0x09, 1, temp); 
	LOG_DBG("rc:%d;temp09 :%d", rc,  temp[0]);

	/* For RTU */
	modbus_close(ctx);
	modbus_free(ctx);
}

