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

def movingAverage(signal, numPoints):
    if numPoints == 0:
        return signal
    buffer = [0] * numPoints
    result = []
    for i in range(len(signal)):
        buffer.pop(0) #remove the oldest value
        buffer.append(signal[i])
        result.append(sum(buffer)/numPoints)
    return result

sigs = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]
Xs = [400,400,0,40]
fig, axs = plt.subplots(len(sigs), 4, figsize=(20,20))
rowIndex = 0;
for sig, X in zip(sigs, Xs):
    t, signal = readData(sig)
    signalAvg = movingAverage(signal, X)
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
column_titles = ['signal', 'smoothed signal', 'FFT on signal', 'FFT on smoothed signal']
for ax, col in zip(axs[0], column_titles):
    ax.set_title(col, fontsize=12, pad=20)

#show the moving average buffer size X
for ax, X in zip(axs[:,0], Xs):
    ax.set_ylabel(f"X={X}", rotation=0, labelpad=60, fontsize=12)

plt.show()

