import serial
import time
import json

pico_serial_port = '/dev/tty.SLAB_USBtoUART'
baud_rate = 115200 

class Client:

    def __init__(self):
        self.state = "connect"
        self.prevState = None
        self.iterCount = 0
        self.lastReleaseSpeed = 0

        self.transitions = {
            "connect": "up",
            "up": "prime",
            "prime": "push",
            "push": "accel",
            "accel":"sample",
            "sample": "release",
            "release": "wait",
            "wait": "up"
        }
        self.ser = serial.Serial(pico_serial_port, baud_rate, timeout=1)

        with open('trials.json', 'r') as file:
            self.data = json.load(file)

        time.sleep(2)
        self.stateStart = time.time_ns()

    def nextState(self):
        if self.transitions[self.state] != self.state:
            self.stateStart = time.time_ns();
        self.prevState = self.state
        self.iterCount = 0
 
        self.state = self.transitions[self.state]
    def timeInState(self):
        return (time.time_ns()-self.stateStart)/(10**9)

    def firstTime(self):
        return self.iterCount == 1
    
    def getLine(self):
        if self.ser.in_waiting > 0:
            return str(self.ser.readline().decode('utf-8').rstrip())
    
    def sendControl(self, motor, servo, mark=0):
        self.ser.write((f"({str(motor)},{str(servo)},{str(mark)})\n").encode())

    def run(self):
        try:
            while True:
                self.iterCount += 1

                if self.state == "connect":
                        if self.firstTime():
                            print(f"Connected.")
                        self.nextState()
                elif self.state == "up":
                    if self.firstTime():
                        self.sendControl(0,95)
                    input("Place top.")
                    self.nextState()
                elif self.state == "prime":
                    if self.firstTime():
                        self.sendControl(0,90)
                    input("Press to push.")
                    self.nextState()
                elif self.state == "push":
                    if self.firstTime():
                        self.sendControl(0,88)
                    input("Press to accel.")
                    self.nextState()
                elif self.state == "accel":
                    if self.firstTime():
                        self.sendControl(20000,88)
                    elif self.timeInState() >= 5:
                        self.sendControl(20000,88, mark=1)
                        self.nextState() 
                elif self.state == "sample":
                    if self.timeInState() >= 1:
                        self.nextState() 
                elif self.state == "release":
                    if self.firstTime():
                        self.trialStartTime = time.time_ns()
                        self.sendControl(0,110)
                    if self.timeInState() > 1:
                        if self.ser.in_waiting > 0:
                            line = self.ser.readline().decode('utf-8').rstrip()
                            self.lastReleaseSpeed = float(line)
                            print(f"Speed at release: {line} rpm.")
                        self.nextState()
                elif self.state == "wait":
                    if self.firstTime():
                        self.sendControl(0,110)
                    input("Press to end.")
                    elapsed = time.time_ns() - self.trialStartTime
                    desig = input(f"{elapsed/10**9} seconds. Enter top label: ")
                    self.data[time.time()] = {"type":desig, "elapsed":elapsed, "release":self.lastReleaseSpeed}
                    print("Wrote data.")
                    self.nextState()
                
                self.flag = False

        except KeyboardInterrupt:
            print("Closing serial connection.")

        finally:
            self.ser.close()
            with open('trials.json', 'w') as file:
                json.dump(self.data, file)

if __name__ == "__main__":
    client = Client()
    client.run()