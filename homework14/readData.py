import csv

filename = "sigB.csv"
t = [] #column 0
data1 = [] #column 1

#read the data
with open(filename) as f:
    reader = csv.reader(f)
    for row in reader:
        t.append(float(row[0])) 
        data1.append(float(row[1]))

#print the data that was read
for i in range(len(t)):
    print(f"{str(t[i])},{str(data1[i])}")