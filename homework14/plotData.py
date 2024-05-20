import csv
import matplotlib.pyplot as plt

def getSampleRate(timeValues):
    return (len(timeValues)-1)/(timeValues[-1]-timeValues[0])

filename = "sigD.csv"
t = [] #column 0
data1 = [] #column 1

#read the data
with open(filename) as f:
    reader = csv.reader(f)
    for row in reader:
        t.append(float(row[0])) 
        data1.append(float(row[1]))

plt.plot(t, data1)
plt.title(filename)
plt.xlabel("time")
plt.ylabel("signal value")

plt.show()
print(f"The sample rate is {getSampleRate(t)} samples/sec")