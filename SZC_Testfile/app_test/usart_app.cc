#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <serialport.h>

int main() {
    const char* portname = "/dev/ttyUSB0"; // 修改为实际的串口设备名
    struct sp_port* serial_port;
    int error;

    error = sp_get_port_by_name(portname, &serial_port);
    if (error != SP_OK) {
        fprintf(stderr, "Error finding serial port: %s\n", sp_get_error_message(error));
        return 1;
    }

    error = sp_open(serial_port, SP_MODE_READ_WRITE);
    if (error != SP_OK) {
        fprintf(stderr, "Error opening serial port: %s\n", sp_get_error_message(error));
        return 1;
    }

    // 发送数据
    const char* message = "Hello, STM32!";
    sp_nonblocking_write(serial_port, message, strlen(message));

    // 接收数据
    char buffer[256];
    int bytes_read = sp_blocking_read(serial_port, buffer, sizeof(buffer), 1000);
    if (bytes_read > 0) {
        printf("Received: %.*s\n", bytes_read, buffer);
    }

    sp_close(serial_port);
    sp_free_port(serial_port);

    return 0;
}
