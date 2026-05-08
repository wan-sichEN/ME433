import matplotlib.pyplot as plt
import numpy as np

import csv

t = [] # column 1
dataD = [] # column 2

with open('sigD.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        dataD.append(float(row[1])) # second column

Fs = 1/(t[1] - t[0]) # sample rate (10 kHz)
y = dataD
n = len(y) # length of the signal
k = np.arange(n) 
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

# MAF
maf_data = []
X = 25
for i in range(n):
    avg = 0
    for j in range(X):
        if i-j >= 0: # treats earlier X values as zeros
            avg += dataD[i-j]
    avg = avg/X
    maf_data.append(avg)

Y_maf = np.fft.fft(maf_data)/n # fft computing and normalization
Y_maf = Y_maf[range(int(n/2))]

# Comparing unfiltered and MAF
fig, (ax1, ax2) = plt.subplots(2, 1)

ax1.plot(t,y,'k')
ax1.plot(t, maf_data, 'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')

ax2.loglog(frq,abs(Y),'k') # plotting the fft
ax2.loglog(frq, abs(Y_maf), 'r')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')

ax1.legend(['Original', 'MAF'])
ax2.legend(['Original', 'MAF'])
fig.suptitle(f"Signal D MAF: {X} Data Points Averaged")
plt.show()

#IIR
iir_data = [0]*n
iir_data[0] = dataD[0]

A = .98
B = 1 - A
for i in range(1,n):
    iir_data[i] = A*iir_data[i-1] + B*dataD[i]

Y_iir = np.fft.fft(iir_data)/n # fft computing and normalization
Y_iir = Y_iir[range(int(n/2))]

# Comparing unfiltered and IIR
fig, (ax1, ax2) = plt.subplots(2, 1)

ax1.plot(t,y,'k')
ax1.plot(t, maf_data, 'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')

ax2.loglog(frq,abs(Y),'k') # plotting the fft
ax2.loglog(frq, abs(Y_iir), 'r')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')

ax1.legend(['Original', 'IIR'])
ax2.legend(['Original', 'IIR'])
fig.suptitle(f"Signal D IIR: A = {A}, B = {B}")
plt.show()

h = [
    0.000000000000000000,
    -0.000009623869967983,
    -0.000020818032562861,
    0.000000000000000000,
    0.000098764882327767,
    0.000336930368822832,
    0.000793024668008847,
    0.001561513680266334,
    0.002748213488818310,
    0.004462005574505771,
    0.006803237403779138,
    0.009849838855197846,
    0.013642742912103344,
    0.018172566203833222,
    0.023369604958958578,
    0.029098996652861222,
    0.035162395254775616,
    0.041306763724159604,
    0.047239996826458276,
    0.052652172975869084,
    0.057240427192777527,
    0.060734858993446689,
    0.062922630115965453,
    0.063667514339190559,
    0.062922630115965467,
    0.060734858993446689,
    0.057240427192777520,
    0.052652172975869105,
    0.047239996826458283,
    0.041306763724159604,
    0.035162395254775616,
    0.029098996652861243,
    0.023369604958958581,
    0.018172566203833219,
    0.013642742912103338,
    0.009849838855197850,
    0.006803237403779142,
    0.004462005574505773,
    0.002748213488818313,
    0.001561513680266335,
    0.000793024668008847,
    0.000336930368822832,
    0.000098764882327767,
    0.000000000000000000,
    -0.000020818032562861,
    -0.000009623869967983,
    0.000000000000000000,
]

fir_data = []
n_fir = len(h)

for i in range(n):
    avg = 0
    for j in range(n_fir):
        if i-j >= 0: # treats earlier X values as zeros
            avg += h[j]*dataD[i-j]
    fir_data.append(avg)

Y_fir = np.fft.fft(fir_data)/n # fft computing and normalization
Y_fir = Y_fir[range(int(n/2))]

# Comparing unfiltered and FIR
fig, (ax1, ax2) = plt.subplots(2, 1)

ax1.plot(t,y,'k')
ax1.plot(t, fir_data, 'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')

ax2.loglog(frq,abs(Y),'k') # plotting the fft
ax2.loglog(frq, abs(Y_fir), 'r')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')

ax1.legend(['Original', 'FIR'])
ax2.legend(['Original', 'FIR'])
fig.suptitle(f"Signal D FIR: f_cutoff = 10 Hz, bandwidth = 50 Hz, {n_fir} Coefficients")
plt.show()