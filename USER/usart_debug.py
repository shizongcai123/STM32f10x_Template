# import serial

# # ser = serial.Serial('COMx', baudrate=115200)  # 将'COMx'替换为你的串口名称

# # while True:
# #     data = ser.read(2)  # 读取两个字节数据
# #     if len(data) == 2:
# #         high_byte, low_byte = data
# #         data16 = (high_byte << 8) | low_byte
# #         print(f'Received 16-bit data: {data16}')
# import serial

# ser = serial.Serial(
#     port='/dev/ttyUSB1',  # 串口设备名称，根据实际情况修改
#     baudrate=9600,      # 波特率，必须与STM32配置一致
#     timeout=1             # 读取超时时间，根据需要进行调整
# )

# try:
#     message_buffer = ""  # 用于缓存接收的数据
#     while True:
#         print("beging usart_debug_exam")
#         data = ser.read(2) # 逐个字节读取数据

#         # print("data = = ",data)
#         # if char:
#         #     print("ok char")
#         #     if char == '\n':
#         #         # 遇到换行符，处理一条消息
#         #         print("Received Message==:", message_buffer)
#         #         message_buffer = ""  # 清空缓存
#         #     else:
#         #         message_buffer += char  # 将字符添加到缓存中
# except KeyboardInterrupt:
#     ser.close()

FLAG_HEAD0 = b'\xce'
FLAG_HEAD1 = b'\xfa'
import struct
import time

import serial

# 打开串口连接，根据您的串口设置进行调整
ser = serial.Serial(
    port='/dev/ttyUSB0',  # 串口设备名称，根据实际情况修改
    baudrate=9600,      # 波特率，必须与STM32配置一致
    timeout=1             # 读取超时时间，根据需要进行调整
)
# print("begin")
# while True:
#     if ser.in_waiting > 1:  # 检查是否有足够的数据可用
#         lowByte = ser.read(1) # 读取低8位字节
#         highByte = ser.read(1) # 读取高8位字节
#         value = int.from_bytes(highByte + lowByte, byteorder='little', signed=True)  # 合并两个字节以获得int16_t值
#         print("Received int16_t value:", value)



print("begin====")
while True:
        firstbyte1 = ser.read(1) # 读取8位字节
        firstbyte2 = ser.read(1)
        if(firstbyte1 == FLAG_HEAD0 and firstbyte2 == FLAG_HEAD1):
            # lowByte = ser.read(1) # 读取低8位字节
            # highByte = ser.read(1) # 读取高8位字节
            # x_value = int.from_bytes(highByte + lowByte, byteorder='big', signed=True)  # 合并两个字节以获得int16_t值
            # print("Received int16_t value x:", x_value)
            # lowByte = ser.read(1) # 读取低8位字节
            # highByte = ser.read(1) # 读取高8位字节
            # y_value = int.from_bytes(highByte + lowByte, byteorder='big', signed=True)  # 合并两个字节以获得int16_t值
            # print("Received int16_t value: y", y_value)
            # lowByte = ser.read(1) # 读取低8位字节
            # highByte = ser.read(1) # 读取高8位字节
            # z_value = int.from_bytes(highByte + lowByte, byteorder='big', signed=True)  # 合并两个字节以获得int16_t值
            # print("Received int16_t value: z", z_value)

                    accxyz = ser.read(6)
                    accx, accy, accz = struct.unpack('hhh', accxyz)
                    # 打印解包后的数据
                    print("AccX:", accx)
                    print("AccY:", accy)
                    print("AccZ:", accz)
            # value = int.from_bytes(highByte + lowByte, byteorder='little', signed=True)  # 合并两个字节以获得int16_t值
            # print("Received int16_t value:", value)