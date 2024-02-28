# SERIAL COMMUNICATION; CONTAINTS CLASSES OF ALL VARIABLES AND FUNCTIONS FOR SERIAL COMMUNICATION

import serial.tools.list_ports  # pip install pyserial

class Serial_COM():
    def __init__(self):
        '''
        Initializing the main varialbles for the serial data
        '''
        pass

    def find_Teensy(self):
        '''
        Method to automatically find the teensy com port
        '''
        # search computer for serial ports
        self.teensy_ports = []
        print("\nsearching for serial ports...")
        ports = serial.tools.list_ports.comports()
        if len(ports) > 0:
            for port, desc, hwid in sorted(ports):
                print("COM PORT FOUND:", "[{}][{}][{}]".format(port, desc, hwid))
                if "13960230" or "14200900" in hwid:
                    self.teensy_ports.append(port)
                # Virtual port used for testing
                if "CNCB0" in hwid or "CNCA0" in hwid or "CNCA1" in hwid or "CNCB1" in hwid:
                    self.teensy_ports.append(port)
            print("searching for teensy ports...")
            if len(self.teensy_ports) > 0:
                for port in self.teensy_ports:
                    print("TEENSY PORT FOUND:", port)
            else:
                print("NO TEENSY PORT FOUND: make sure a teensy is plugged in and refresh.")
        else:
            print("NO COM PORT FOUND: make sure a serial port is plugged in and refresh.")
    
    def SerialOpen(self, ComGUI):
        '''
        Method to setup the serial connection and make sure to go for the next only 
        if the connection is done properly
        '''
        try:
            self.ser.is_open
        except:
            PORT = ComGUI.clicked_port.get()
            BAUD = ComGUI.clicked_baud.get()
            self.ser = serial.Serial()
            self.ser.baudrate = BAUD
            self.ser.port = PORT
            # TESTING (WAS 0.1)
            self.ser.timeout = 0.1
        try:
            if self.ser.is_open:
                print("Already Open")
                self.ser.status = True
            else:
                PORT = ComGUI.clicked_port.get()
                BAUD = ComGUI.clicked_baud.get()
                self.ser = serial.Serial()
                self.ser.baudrate = BAUD
                self.ser.port = PORT
                # TESTING (WAS 0.01)
                self.ser.timeout = 0.01

                self.ser.open()
                self.ser.status = True
        except:
            self.ser.status = False

    def SerialClose(self):
        '''
        Method used to close the communication
        '''
        try:
            self.ser.is_open
            # close both valves first
            # perhaps add a delay inbetween commands??
            self.actuate("v1", "CLOSE")
            self.actuate("v2", "CLOSE")
            self.ser.close()
            self.ser.status = False
        except:
            self.ser.status = False

    def read_sensors(self):
        '''
        Method to read analog data from teensy
        '''
        self.teensyData = self.ser.readline()
        self.teensyData = self.teensyData.decode("ascii").rstrip()
        return self.teensyData.split(',')
    
    def actuate(self, valve, command):
        '''
        Method to actuate the MOSFETs that control the solenoid valves
        '''
        # use input data to figure out what to write to the teensy
        if valve == "v1":
            if command == "OPEN":
                case = "0"                  # pin 2, high
            else:
                case = "1"                  # pin 2, low
        else:
            if command == "OPEN":
                case = "2"                  # pin 9, high
            else:
                case = "3"                  # pin 9, low
        self.ser.write(case.encode())

        # report logic to terminal
        print(valve, command + ":", case.encode())
