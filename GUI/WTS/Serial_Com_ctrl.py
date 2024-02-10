import serial.tools.list_ports

class SerialCtrl():
    def __init__(self):
        self.com_list = []

    def getCOMList(self):
        ports = serial.tools.list_ports.comports()
        self.com_list = [com[0] for com in ports]
        self.com_list.insert(0,'-')
    def SerialOpen(self, gui):
        try:
            self.ser.is_open
        except:
            PORT = gui.clicked_com.get()
            BAUD = gui.clicked_bd.get()
            self.ser = serial.Serial()
            self.ser.baudrate = BAUD
            self.ser.port = PORT
            self.ser.timeout = 0.1
        
        try:
            if self.ser.is_open:
                self.ser.status = True
            else:
                PORT = gui.clicked_com.get()
                BAUD = gui.clicked_bd.get()
                self.ser = serial.Serial()
                self.ser.baudrate = BAUD
                self.ser.port = PORT
                self.ser.timeout = 0.1
                self.ser.open()
                self.ser.status = True
        except:
            self.ser.status = False
    
    def SerialClose(self):
        try:
            self.ser.is_open
            self.ser.close()
            self.ser.status = False
        except:
            self.ser.status = False

# excecutes when in main
if __name__ == "__main__":
    SerialCtrl()
