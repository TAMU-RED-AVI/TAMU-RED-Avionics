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
                if "13960230" in hwid:
                    self.teensy_ports.append(port)
            print("\nsearching for teensy ports...")
            if len(self.teensy_ports) > 0:
                for port in self.teensy_ports:
                    print("TEENSY PORT FOUND:", port)
            else:
                print("NO TEENSY PORT FOUND: make sure a teensy is plugged in and refresh.", "\n")
        else:
            print("NO COM PORT FOUND: make sure a serial port is plugged in and refresh.", "\n")
    
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
                self.ser.timeout = 0.01
                self.ser.open()
                self.ser.status = True
        except:
            self.ser.status = False

    def SerialClose(self, ComGUI):
        '''
        Method used to close the UART communication
        '''
        try:
            self.ser.is_open
            self.ser.close()
            self.ser.status = False
        except:
            self.ser.status = False

    def read_sensors(self):
        '''
        Method to read analog data from teensy
        '''
        # add logic to repeat reading the data
        self.ser = serial.Serial("COM5", baudrate=9600, timeout=1)
        self.teensyData = self.ser.readline().decode('ascii')
        print(self.teensyData)
        # add logic to store the teensy data