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
   1. In the `uart_send()` function in `uart_device.c`, 
   2. add a 10ms delay after sending the command 
   (based on testing, it was found that 10ms is the minimum wait time; if it's less than 10ms, data reception fails).
   3. Modbus 串行传输时按如下顺序：括号中的字符。
      1. Modbus 主机组织信息.
      2. 检查主机设备 Modbus RTS 和 CTS 的状态。(A)
         若 RTS 和 CTS 跳接在一起，可忽略该时间。  对 J478 modem,该时间约 5ms。
      3. 向从机发送查询信息(B) 
         Use the following formula to estimate the transmission time:
         Time (ms)  =    (1000  X  (character count)  X  (bits per character) ) / Baud Rate
      4. 从机处理查询数据(C) (D) 
         (C)   The Modbus message is processed at the end of the controller scan. 
               worst-case delay is one scan time, which occurs if the controller has just begun a new scan.
               The average delay is 0.5 scan time.
               The time allotted for servicing Modbus ports at the end of the controller scan (before beginning a new scan)
               depends upon the controller modle. 
               For 484 controllers the time is approximately 1.5 ms. The modbus port is available on a contention basis with J470/J474/J475 that is present.
               For 584 and 984 controllers the time is approximately 1.5 ms for each Modbus port. The port are serviced sequentially, starting with port 1.
               For 184/384 controllers the time varies according to the amount of data being handled. 
                  It ranges from a minimum of 0.5 ms to a maximum of about 6.0 ms, or 7.0 ms(for 800 coils).
                  If a programming panel is currently being used with the controller, the modbus port is locked out.
         (D)   Modbus functions 1 through 4, 15, and 16 permit the master to request more data than can be processed 
                  during the time alloted for servicing the slave's Modbus port. If the slave cannot process all of the data,
                  It will buffer the data and process it at the end of the subsequently scans.
                  The amount of the data that can be processed during one service period at the Modbus port is as follows:、

                  | Name      | Discretes | Registers |
                  |-----------|-----------|-----------|
                  | Micro 84  | 16        | 4         |
                  | 184/384   | 800       | 100       |
                  | 484       | 32        | 16        |
                  | 584       | 64        | 32        |
                  | 984A/B/X  | 64        | 32        |
                  | 984-X8X   | 1000      | 125       |


                  Note:  `984-X8X` refers to 984 slot-mount models(984-385, -685, etc).
                  For the 884, the processing time for multiple data is as follows:
                     Read 768 coils:   14 scans       Force single coil:   3  scans
                     Read 256 inputs:  7  scans       Preset registers:    3  scans
                     Read 125 output registers: 5  scans  Force 768 coils: 18 scans
                     Read 125 input registers:  8  scans    Preset 100 registers:   10 scans
         (E)   LRC calculation time is less than 1ms.
               CRC calculation time is about 0.3ms for each 8 bits of data to be returned in the response.



      5. 从机计算一个错误校验区(E) 
      6. 检查从机设备 RTS 和 CTS,modem 的状态(A) 
      7. 向主机返回响应信息(B) 
      8. 主机按从机响应的数据处理


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
