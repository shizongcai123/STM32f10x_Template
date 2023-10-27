import serial

ser = serial.Serial('COMx', baudrate=115200)  # 将'COMx'替换为你的串口名称

while True:
    data = ser.read(2)  # 读取两个字节数据
    if len(data) == 2:
        high_byte, low_byte = data
        data16 = (high_byte << 8) | low_byte
        print(f'Received 16-bit data: {data16}')
