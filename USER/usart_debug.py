import struct
import time

import serial

# 打开串口连接，根据您的串口设置进行调整
ser = serial.Serial(
    port='/dev/ttyUSB0',  # 串口设备名称，根据实际情况修改
    baudrate=9600,      # 波特率，必须与STM32配置一致
    timeout=1             # 读取超时时间，根据需要进行调整
)
FLAG_HEAD0 = b'\xce'
FLAG_HEAD1 = b'\xfa'
print("begin====")
while True:
        firstbyte1 = ser.read(1) # 读取8位字节
        if(firstbyte1 == FLAG_HEAD0):#先判断HEAD0，当单HEAD0不满足时就不需要进行下一步了。而不是同时判断HEAD0和HEAD1
            firstbyte2 = ser.read(1)
            if( firstbyte2 == FLAG_HEAD1):
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
                mpudata = ser.read(12)
                accx, accy, accz,gyrx,gyry,gyrz = struct.unpack('hhhhhh', mpudata)
                # 打印解包后的数据
                print("======Acc======")
                print("AccX:", accx)
                print("AccY:", accy)
                print("AccZ:", accz)
                print("======Gyr======")
                print("GyrX:", gyrx)
                print("GyrY:", gyry)
                print("GyrZ:", gyrz)