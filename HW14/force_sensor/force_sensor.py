import matplotlib.pyplot as plt
import numpy as np

t = [] # column 1
raw_data = [] # column 2
iir_data = [] # column 3

# collecting data
import serial

ser = serial.Serial('COM8', 115200)  # adjust port as needed

num_points = 500
ser.write(f"{num_points}\n".encode())

print("Collecting Data")

for _ in range(num_points):
    line = ser.readline().decode().strip()
    timestamp, raw, iir = line.split()
    t.append(int(timestamp) / 1000)  # ms to seconds
    raw_data.append(int(raw))
    iir_data.append(int(iir))

t = [x - t[0] for x in t] # sets first t value to 0

# raw data FFT
Fs = 1 / np.mean(np.diff(t))  # t in seconds (should be around 25*2 us = 50 us)
print(f"Sampling Rate = {Fs} Hz")
y = raw_data
n = len(y) # length of the signal
k = np.arange(n) 
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

# filtered data FFT
Y_iir = np.fft.fft(iir_data)/n # fft computing and normalization
Y_iir = Y_iir[range(int(n/2))]

# plots
fig, (ax1, ax2) = plt.subplots(2, 1)

ax1.plot(t,y,'k')
ax1.plot(t, iir_data, 'r')
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Amplitude')

ax2.loglog(frq,abs(Y),'k') # plotting the fft
ax2.loglog(frq, abs(Y_iir), 'r')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')

ax1.legend(['Original', 'IIR'])
ax2.legend(['Original', 'IIR'])
fig.suptitle(f"Data and FFT Comparison: Raw vs Filtered ({num_points} Data Points Collected)")
plt.show()