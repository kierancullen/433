import matplotlib.pyplot as plt
import numpy as np
import csv

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

def IIR(signal, A):
    result = []
    for i in range(len(signal)):
        if i == 0:
            result.append((1-A)*signal[i]) #as though the current running average is 0
        else:
            result.append(A*result[i-1]+(1-A)*signal[i])
    return result

sigs = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]
As = [0.995, 0.995, 0, 0.9]
fig, axs = plt.subplots(len(sigs), 4, figsize=(20,20))
rowIndex = 0;
for sig, A in zip(sigs, As):
    t, signal = readData(sig)
    signalAvg = IIR(signal, A)
    frq, Y = doFFT(t, signal)
    frq2, Y2 = doFFT(t, signalAvg)

    axs[rowIndex,0].plot(t, signal, 'k')
    axs[rowIndex,2].loglog(frq, abs(Y),'k')

    axs[rowIndex,1].plot(t, signalAvg, 'r')
    axs[rowIndex,3].loglog(frq2, abs(Y2),'r')

    axs[rowIndex,0].set_xlabel('time')
    axs[rowIndex,0].set_ylabel('amplitude')
    axs[rowIndex,1].set_xlabel('time')
    axs[rowIndex,1].set_ylabel('amplitude')
    axs[rowIndex,2].set_xlabel('frequency (Hz)')
    axs[rowIndex,2].set_ylabel('|Y|')
    axs[rowIndex,3].set_xlabel('frequency (Hz)')
    axs[rowIndex,3].set_ylabel('|Y|')
    rowIndex+=1;

#column titles
column_titles = ['signal', 'IIR-smoothed signal', 'FFT on signal', 'FFT on IIR-smoothed signal']
for ax, col in zip(axs[0], column_titles):
    ax.set_title(col, fontsize=12, pad=20)

#show the moving average buffer size X
for ax, A in zip(axs[:,0], As):
    ax.set_ylabel(f"A={A:.3f}, B={(1-A):.3f}", rotation=0, labelpad=60, fontsize=12)

plt.show()

