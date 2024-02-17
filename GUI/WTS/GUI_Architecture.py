# GUI ARCHITECTURE; CONTAINTS CLASSES OF ALL VARIABLES AND FUNCTIONS FOR THE GUI

from tkinter import *
from tkinter import messagebox
from PIL import ImageTk, Image

class GUI_Root():
    def __init__(self):
        '''
        Initiallizing the root GUI
        '''
        self.root = Tk()
        self.root.title("WTS GUI")
        self.root.geometry("335x130")
        self.root.config(bg="#000")
        # RED logo
        self.RED_logo = Image.open("G:/Shared drives/TAMU Rocket Engine Design/02 Subsystems - General/04 AVI - Avionics & Controls/03 WTS/WTS GUI/RED_logo.png")
        self.RED_logo = ImageTk.PhotoImage(self.RED_logo)
        self.RED_logo_label = Label(self.root, image=self.RED_logo, bg="#000")
        self.RED_logo_label.grid(row=0, column=0, sticky=W) 


# GUI for connecting to the Teensy
class GUI_COM():
    def __init__(self, root, serial):
        '''
        Initialize the connection GUI and related widgets
        '''
        self.root = root
        self.serial = serial
        # frame
        self.frame = LabelFrame(root, text="Teensy Connection", padx=5, pady=5, bg="#000", fg="#fff")
        # teensy port and baud rate labels
        self.port_label = Label(self.frame, text="COM Port: ", width=15, anchor="w", bg="#000", fg="#fff")
        self.baud_label = Label(self.frame, text="Baud Rate: ", width=15, anchor="w", bg="#000", fg="#fff")
        # port and baud rate options
        self.get_port_options()
        self.get_baud_options()
        # refresh button and connect button
        self.btn_refresh = Button(self.frame, text="Refresh", width=10,bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", command=self.refresh_click)
        self.btn_connect = Button(self.frame, text="Connect", width=10, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", command=self.connect_click, state="disabled")
        # display all the frame and its widgets
        self.display()
    
    def display(self):
        '''
        Method to display all the frame and its widgets
        '''  
        # frame location
        self.frame.grid(row=1, column=0, rowspan=3, columnspan=3, padx=5, pady=5)
        # com port and baud rate labels
        self.port_label.grid(column=0,row=1)
        self.baud_label.grid(column=0,row=2)
        # drop down menus
        self.drop_ports.grid(column=2,row=1, sticky=E, padx=3)
        self.drop_bauds.grid(column=2,row=2, padx=3)
        # buttons
        self.btn_refresh.grid(column=3, row=1)
        self.btn_connect.grid(column=3, row=2)

    def get_port_options(self):
        '''
         Method to Get the available COMs connected to the PC
         and find the teensy
        '''
        self.serial.find_Teensy()
        self.clicked_port = StringVar()
        self.serial.teensy_ports.insert(0, "-")
        self.clicked_port.set(self.serial.teensy_ports[0])
        self.drop_ports = OptionMenu(self.frame, self.clicked_port, *self.serial.teensy_ports, command=self.connect_btn_ctrl)
        self.drop_ports.config(width=11, bg="#000", fg="#fff", highlightbackground="black", highlightcolor="grey")

    def get_baud_options(self):
        '''
         Method to Get the available COMs connected to the PC
         and find the teensy
        '''
        baud_rates = ["-", "300", "600", "1200", "2400", "4800",
                      "9600", "14400", "19200", "28800", "38400",
                      "56000", "57600", "115200", "128000", "256000"]
        self.clicked_baud = StringVar()
        self.clicked_baud.set(baud_rates[0])
        self.drop_bauds = OptionMenu(self.frame, self.clicked_baud, *baud_rates, command=self.connect_btn_ctrl)
        self.drop_bauds.config(width=11, bg="#000", fg="#fff", highlightbackground="black", highlightcolor="grey")

    def connect_btn_ctrl(self, widget):
        '''
        Mehtod to keep the connect button disabled if all the 
        conditions are not cleared
        '''
        # Checking the logic consistency to keep the connection btn
        if "-" in self.clicked_baud.get() or "-" in self.clicked_port.get():
            self.btn_connect["state"] = "disabled"
        else:
            self.btn_connect["state"] = "normal"

    def refresh_click(self):
        '''
        Mehtod to refresh the available teensy ports
        '''
        # Get the Widget destroyed
        self.drop_ports.destroy()
        # Refresh the list of available Coms
        self.get_port_options()
        # display the new menu
        self.drop_ports.grid(column=2,row=1, padx=3)

    def connect_click(self):
        '''
        Method that Updates the GUI during connect / disconnect status
        Manage serials and data flows during connect / disconnect status
        '''
        if self.btn_connect["text"] in "Connect":
            # Start the serial communication
            self.serial.SerialOpen(self)

            # If connection established move on
            if self.serial.ser.status:
                # Update the COM manager
                self.btn_connect["text"] = "Disconnect"
                self.btn_refresh["state"] = "disable"
                self.drop_bauds["state"] = "disable"
                self.drop_ports["state"] = "disable"
                InfoMsg = f"Successful connection using {self.clicked_port.get()}"
                messagebox.showinfo("showinfo", InfoMsg)

                # display the control GUIS
                self.CTRL = GUI_CTRL(self.root, self.serial)
                self.DAQ = GUI_DAQ(self.root, self.serial)

            # If connection not establish (i.e. the serial port is already being used by another program)
            else:
                ErrorMsg = f"Failure to estabish connection using {self.clicked_port.get()} "
                messagebox.showerror("showerror", ErrorMsg)
        else:
            # Close the serial communication
            self.serial.SerialClose(self)

            # Closing the GUI for the controls and daq
            self.CTRL.close()
            self.DAQ.close()

            # reset connection box
            InfoMsg = f"Connection using {self.clicked_port.get()} is now closed"
            messagebox.showwarning("showinfo", InfoMsg)
            self.btn_connect["text"] = "Connect"
            self.btn_refresh["state"] = "normal"
            self.drop_bauds["state"] = "normal"
            self.drop_ports["state"] = "normal"

# GUI for controlling the solenoid valves
class GUI_CTRL():
    def __init__(self, root, serial):
        '''
        Initialize the CTRL GUI and related widgets
        '''
        self.root = root
        self.serial = serial

        # frame
        self.frame = LabelFrame(root, text="Control Information", padx=5, pady=5, bg="#000", fg="#fff")
        # valva labels
        self.val1_label = Label(self.frame, text="VALVE1 Control: ", width=15, anchor="w", bg="#000", fg="#fff")
        self.val2_label = Label(self.frame, text="VALVE2 Control: ", width=15, anchor="w", bg="#000", fg="#fff")
        # on/off buttons
        self.btn_val1_on = Button(self.frame, text="OPEN", width=6, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.v1_ON)
        self.btn_val2_on = Button(self.frame, text="OPEN", width=6, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.v2_ON)
        self.btn_val1_off = Button(self.frame, text="CLOSE", width=6, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.v1_OFF)
        self.btn_val2_off = Button(self.frame, text="CLOSE", width=6, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.v2_OFF)
        # save button
        self.btn_save = Button(self.frame, text="Save Data", width=10,bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.ctrl_SAVE)

        # display all the frame and its widgets
        self.display()
    
    def display(self):
        '''
        Method to display all the frame and its widgets
        '''
        # frame location
        self.frame.grid(row=4, column=0, rowspan=3, columnspan=3, padx=5, pady=5)
        # valva labels
        self.val1_label.grid(column=0,row=1)
        self.val2_label.grid(column=0,row=2)
        # on/off buttons
        self.btn_val1_on.grid(column=1,row=1, padx=0, pady=5)
        self.btn_val2_on.grid(column=1,row=2, padx=0, pady=0)
        self.btn_val1_off.grid(column=2, row=1, padx=5, pady=5)
        self.btn_val2_off.grid(column=2, row=2, padx=5, pady=0)
        # button
        self.btn_save.grid(column=3, row=1)

    def close(self):
        '''
        Method to close the controls GUI and destorys the widgets
        '''
        # destroy all the elements so they are not kept in system memory
        for widget in self.frame.winfo_children():
            widget.destroy()
        self.frame.destroy()
        self.root.geometry("335x130")

    def v1_ON(self):
        '''
        Method to turn valve 1 on
        '''  
        print("valve 1 on")
        self.btn_val1_on["state"] = "disable"
        self.btn_val1_off["state"] = "normal"
        pass

    def v2_ON(self):
        '''
        Method to turn valve 2 on
        '''  
        print("valve 2 on")
        self.btn_val2_on["state"] = "disable"
        self.btn_val2_off["state"] = "normal"
        pass

    def v1_OFF(self):
        '''
        Method to turn valve 1 off
        '''    
        print("valve 1 off")
        self.btn_val1_on["state"] = "normal"
        self.btn_val1_off["state"] = "disable"
        pass

    def v2_OFF(self):
        '''
        Method to turn valve 2 off
        '''
        print("valve 2 off")
        self.btn_val2_on["state"] = "normal"
        self.btn_val2_off["state"] = "disable"
        pass

    def ctrl_SAVE(self):
        '''
        Method to save control data into .csv
        '''
        print("save ctrl data")
        pass

# GUI for pressure data aquisition
class GUI_DAQ():
    def __init__(self, root, serial):
        '''
        Initialize the DAQ GUI and related widgets
        '''
        self.root = root
        self.serial = serial
        self.root.geometry("335x365") # update size
        # frame
        self.frame = LabelFrame(root, text="Sensor Information", padx=5, pady=5, bg="#000", fg="#fff")
        # pressure & flow rate data labels
        self.pt1_label = Label(self.frame, text="PT1 Pressure (psia): ", width=21, anchor="w", bg="#000", fg="#fff")
        self.pt2_label = Label(self.frame, text="PT2 Pressure (psia): ", width=21, anchor="w", bg="#000", fg="#fff")
        self.mfr_label = Label(self.frame, text="Mass Flow Rate (kg/s): ", width=21, anchor="w", bg="#000", fg="#fff")
        # pressure & flow rate data values
        self.pt1_current = Label(self.frame, text="-", width=10, anchor="w", bg="#000", fg="#fff")
        self.pt2_current = Label(self.frame, text="-", width=10, anchor="w", bg="#000", fg="#fff")
        self.mfr_current = Label(self.frame, text="-", width=10, anchor="w", bg="#000", fg="#fff")
        # save data button
        self.btn_save = Button(self.frame, text="Save Data", width=10,bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.daq_SAVE)
        # Display all the frame and its widgets
        self.display()

    def display(self):
        '''
        Method to display all the frame and its widgets
        '''
        # frame location
        self.frame.grid(row=7, column=0, rowspan=3, columnspan=3, padx=5, pady=5)
        # sensor data labels
        self.pt1_label.grid(column=0,row=1)
        self.pt2_label.grid(column=0,row=2)
        self.mfr_label.grid(column=0,row=3)
        # sensor data values
        self.pt1_current.grid(column=1,row=1, pady=5)
        self.pt2_current.grid(column=1,row=2, pady=5)
        self.mfr_current.grid(column=1,row=3, pady=5)
        # button
        self.btn_save.grid(column=3, row=1)

    def close(self):
        '''
        Method to close the controls GUI and destorys the widgets
        '''
        # Must destroy all the element so they are not kept in Memory
        for widget in self.frame.winfo_children():
            widget.destroy()
        self.frame.destroy()

    def daq_SAVE(self):
        '''
        Method to save sensor data into .csv
        '''
        print("save sensor data")
        pass

    def read_data(self):
        '''
        Method to display live sensor data into text box
        '''
        pass

# GUI for live graphs of data
class GUI_GRAPHS():
    # add logic for displaying graphs
    pass

