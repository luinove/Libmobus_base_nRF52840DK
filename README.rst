.. zephyr:code-sample:: my_modbus_rtu_client
   :name: my modbus rtu client
   :relevant-api: libmodbus

   Communicate with a Modbus RTU server.

Overview
********

This is a simple application demonstrating a Modbus RTU client implementation
in Zephyr RTOS.

Requirements
************

This sample has been tested with the nRF52840-DK and a modbus-rtu sensor.

Expected result
***************
The task is to read the data sent by the Modbus RTU sensor via UART1, then transmit it to the PC using UART0, and print the received data.

Problem encountered
*******************
1. The received data is not printed completely on the serial port.
   Read using RTT.
2. Error occurred while receiving the response data after sending the Modbus command.
   In the `uart_send()` function in `uart_device.c`, 
   add a 10ms delay after sending the command 
   (based on testing, it was found that 10ms is the minimum wait time; if it's less than 10ms, data reception fails).

How to change the serial port
*****************************
`uart_device.c`: 
The serial transmission and reception are defined in `uart_device.c`.
   1. Select the UART device node: line 13 `#define UART0_DEVICE_NODE DT_CHOSEN(name)`
      1. The `name` is defined in the `chosen` section of the Devicetree overlay file.
      2. The `relevant parameters`(bandrate .etc) are defined in the corresponding interface section of the Devicetree overlay file.
   2. Define private function interfaces: line 73 `static struct UART_Device g_uart0_dev = {"uart0", uart_init, uart_send, uart_recv, uart_flush, DEVICE_DT_GET(UART0_DEVICE_NODE)};``
   3. Place the interface into the index: line 76 `static struct UART_Device *g_uart_devices[] = {&g_uart0_dev, &g_uart1_dev};`

`main.c`
   The Modbus initialization is implemented in `main.c`.
   1. Define a Modbus structure: `modbus_t *ctx;`
   2. Configure Modbus RTU parameters: `ctx = modbus_new_st_rtu("uart1", 9600, 'N', 8, 1);`
   3. Set the device ID: `modbus_set_slave(ctx, 1);`
   4. Read register data: `modbus_read_registers(ctx, 0x06, 1, temp); `
      The meaning of this command is to read 1 register starting from address 06 and store the data into `temp`.
