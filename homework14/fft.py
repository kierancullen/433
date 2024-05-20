import matplotlib.pyplot as plt
import numpy as np

import csv
import matplotlib.pyplot as plt

def getSampleRate(timeValues):
    return (len(timeValues)-1)/(timeValues[-1]-timeValues[0])

def readData(filename):
    t = [] #column 0
    signal = [] #column 1
    #read the data
    with open(filename) as f:
        reader = csv.reader(f)
        for row in reader:
            t.append(float(row[0])) 
            signal.append(float(row[1]))
    return t, signal

def doFFT(t, signal):
    Fs = int(getSampleRate(t)) #sample rate
    Ts = 1.0/Fs; #length of sampling interval
    n = len(signal) #number of signal datapoints
    k = np.arange(n)
    T = n/Fs
    frq = k/T #two sides frequency range
    frq = frq[range(int(n/2))] #one side frequency range
    Y = np.fft.fft(signal)/n # fft computing and normalization
    Y = Y[range(int(n/2))]
    return frq, Y

sigs = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]
fig, axs = plt.subplots(len(sigs), 2, figsize=(20,20))
rowIndex = 0;
for sig in sigs:
    t, signal = readData(sig)
    frq, Y = doFFT(t, signal)
    axs[rowIndex,0].plot(t, signal)
    axs[rowIndex,0].set_xlabel('time')
    axs[rowIndex,0].set_ylabel('amplitude')
    axs[rowIndex,1].loglog(frq,abs(Y),'b')
    axs[rowIndex,1].set_xlabel('frequency (Hz)')
    axs[rowIndex,1].set_ylabel('|Y|')
    rowIndex+=1;

plt.show()


# ax1.plot(t,y,'b')
# ax1.set_xlabel('Time')
# ax1.set_ylabel('Amplitude')
# ax2.loglog(frq,abs(Y),'b') # plotting the fft
# ax2.set_xlabel('Freq (Hz)')
# ax2.set_ylabel('|Y(freq)|')
# plt.show()