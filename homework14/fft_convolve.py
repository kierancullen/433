import matplotlib.pyplot as plt
import numpy as np
import csv
from computeCoeffs import computeCoeffs

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

def applyFilter(signal, h):
    return np.convolve(signal, h, mode='same')

sigs = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]
filterParams = [(10000, 5, 1003),(3300, 3, 697),None,(400, 5, 97)]
fig, axs = plt.subplots(len(sigs), 4, figsize=(20,20))
axs = axs.reshape(len(sigs), 4)
rowIndex = 0;
for sig, params in zip(sigs, filterParams):
    h = computeCoeffs(*params) if params else [1]
    t, signal = readData(sig)
    signalAvg = applyFilter(signal, h)
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
column_titles = ['signal', 'filtered signal', 'FFT on signal', 'FFT on filtered signal']
for ax, col in zip(axs[0], column_titles):
    ax.set_title(col, fontsize=12, pad=20)

#show the filter parameters
for ax, params in zip(axs[:,0], filterParams):
    if params:
        label = f"$f_S={params[0]}$\n$f_L={params[1]}$\n$N={params[2]}$\nBlackman window"
    else:
        label = "No filter"
    ax.set_ylabel(label, rotation=0, labelpad=60, fontsize=12)

plt.show()

