import matplotlib.pyplot as plt
import numpy as np

import csv

t_A = [] # column 1
dataA = [] # column 2
t_B = [] # column 1
dataB = [] # column 2
t_C = [] # column 1
dataC = [] # column 2
t_D = [] # column 1
dataD = [] # column 2

with open('sigA.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t_A.append(float(row[0])) # leftmost column
        dataA.append(float(row[1])) # second column

with open('sigB.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t_B.append(float(row[0])) # leftmost column
        dataB.append(float(row[1])) # second column

with open('sigC.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t_C.append(float(row[0])) # leftmost column
        dataC.append(float(row[1])) # second column

with open('sigD.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t_D.append(float(row[0])) # leftmost column
        dataD.append(float(row[1])) # second column

Fs = 1/(t_A[1] - t_A[0]) # sample rate (10 kHz)
y = dataA
n = len(y) # length of the signal
k = np.arange(n) 
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

# Data A: keep 3 Hz and 25 Hz
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t_A,y,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()

Fs = 1/(t_B[1]-t_B[0]) # sample rate
y = dataB
n = len(y) # length of the signal
k = np.arange(n) 
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

# Data B: keep 0.2 Hz and 3 Hz
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t_B,y,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()

y = dataC
Fs = 1/(t_C[1]-t_C[0]) # sample rate
n = len(y) # length of the signal
k = np.arange(n) 
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

# Data C: Square wave
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t_C,y,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()

Fs = 1/(t_D[1]-t_D[0]) # sample rate
y = dataD
n = len(y) # length of the signal
k = np.arange(n) 
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

# Data D: ??? -> filter everything above 10^1 should work
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t_D,y,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()