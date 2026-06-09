import matplotlib.pyplot as plt 

import serial
ser = serial.Serial('COM7', 115200, timeout=5)
print('Opening port: ')
print(ser.name)

input('Press Enter to collect data...')

# send 'a'
ser.write(b'a\r\n')

index = []
desired = []
actual = []

print('Collecting 400 samples...')

for i in range(400):
    line = ser.read_until(b'\n')
    try:
        parts = line.decode().strip().split('\t')
        index.append(int(parts[0]))
        desired.append(int(parts[1]))
        actual.append(int(parts[2]))
    except:
        print(f'Bad line {i}: {line}')

ser.close()

# plot
plt.figure()
plt.plot(index, desired, 'r-', label='desired')
plt.plot(index, actual, 'b-', label='actual')
plt.xlabel('sample')
plt.ylabel('current (raw)')
plt.title(f'Current Control  KP=0.5 KI=0.5')
plt.legend()
plt.grid(True)
plt.show()