# GUI ARCHITECTURE; CONTAINTS CLASSES OF ALL VARIABLES AND FUNCTIONS FOR THE GUI

from tkinter import *
from tkinter import messagebox
from PIL import ImageTk, Image
import time
import threading

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

class GUI_Root():
    def __init__(self):
        '''
        Initiallizing the root GUI
        '''
        self.root = Tk()
        self.root.title("WTS GUI")
        self.root.geometry("372x130")
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
    
        # protocol to quit GUI when closing window
        self.root.protocol("WM_DELETE_WINDOW", self.quit_GUI)

        # frame
        self.frame = LabelFrame(root, text="Teensy Connection", padx=5, pady=5, bg="#000", fg="#fff")
        # teensy port and baud rate labels
        self.port_label = Label(self.frame, text="COM Port: ", width=18, anchor="w", bg="#000", fg="#fff")
        self.baud_label = Label(self.frame, text="Baud Rate: ", width=18, anchor="w", bg="#000", fg="#fff")
        # port and baud rate options
        self.get_port_options()
        self.get_baud_options()
        # refresh button and connect button
        self.btn_refresh = Button(self.frame, text="Refresh", width=10,bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", command=self.refresh_click)
        self.btn_connect = Button(self.frame, text="Connect", width=10, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", command=self.connect_click, state="disabled")
        # display all the frame and its widgets
        self.display()
    
    def quit_GUI(self):
        '''
        Method to force GUI closing when window is closed.
        '''
        try:
            self.serial.close()
        except:
            pass
        print('closing GUI\n')            
        self.root.quit()
        self.root.destroy()

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
        self.drop_ports.grid(column=1,row=1, sticky=E, padx=3)
        self.drop_bauds.grid(column=1,row=2, padx=3)
        # buttons
        self.btn_refresh.grid(column=2, row=1)
        self.btn_connect.grid(column=2, row=2)

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
        self.drop_ports.config(width=14, bg="#000", fg="#fff", highlightbackground="black", highlightcolor="grey")

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
        self.drop_bauds.config(width=14, bg="#000", fg="#fff", highlightbackground="black", highlightcolor="grey")

    def connect_btn_ctrl(self, widget):
        '''
        Method to keep the connect button disabled if all the 
        conditions are not cleared
        '''
        # Checking the logic consistency to keep the connection btn
        if "-" in self.clicked_baud.get() or "-" in self.clicked_port.get():
            self.btn_connect["state"] = "disabled"
        else:
            self.btn_connect["state"] = "normal"

    def refresh_click(self):
        '''
        Method to refresh the available teensy ports
        '''
        # Get the Widget destroyed
        self.drop_ports.destroy()
        # Refresh the list of available Coms
        self.get_port_options()
        # display the new menu
        self.drop_ports.grid(column=1,row=1, padx=3)

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
                self.DAQ = GUI_DAQ(self.root, self.serial, self.clicked_port.get(), self.CTRL)

                # Update size
                self.root.geometry("1050x423") # update size

                self.DAQ.start()

            # If connection not establish (i.e. the serial port is already being used by another program)
            else:
                ErrorMsg = f"Failure to estabish connection using {self.clicked_port.get()} "
                messagebox.showerror("showerror", ErrorMsg)
        else:
            # Close the serial communication
            self.serial.SerialClose()

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

        # File
        self.control_data = None
        self.recording = False

        # valve control variables
        self.manual = False
        self.v1 = 0
        self.v2 = 0

        # frame
        self.frame = LabelFrame(root, text="Control Information", padx=5, pady=5, bg="#000", fg="#fff")

        # valva labels
        self.val1_label = Label(self.frame, text="VALVE1 Control: ", width=21, anchor="w", bg="#000", fg="#fff")
        self.val2_label = Label(self.frame, text="VALVE2 Control: ", width=21, anchor="w", bg="#000", fg="#fff")
        # on/off buttons
        self.btn_val1_on = Button(self.frame, text="OPEN", width=6, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.v1_OPEN)
        self.btn_val2_on = Button(self.frame, text="OPEN", width=6, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.v2_OPEN)
        self.btn_val1_off = Button(self.frame, text="CLOSE", width=6, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="disable", command=self.v1_CLOSE)
        self.btn_val2_off = Button(self.frame, text="CLOSE", width=6, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="disable", command=self.v2_CLOSE)
        
        # control state labels
        self.ctrl_label = Label(self.frame, text="Control State: ", width=18, anchor="w", bg="#000", fg="#fff")
        # on/off buttons
        self.btn_off = Button(self.frame, text="OFF", width=16, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="disable", command=self.ctrl_off)
        self.btn_pressurize = Button(self.frame, text="PRESSURIZATION", width=16, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.ctrl_pressurize)
        self.btn_operation = Button(self.frame, text="OPERATION", width=16, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.ctrl_operation)

        # save button
        self.btn_save_start = Button(self.frame, text="Start Save", width=10,bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.ctrl_save_start)
        self.btn_save_end = Button(self.frame, text="End Save", width=10, bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="disabled", command=self.ctrl_save_end)

        # display all the frame and its widgets
        self.display()
    
    def display(self):
        '''
        Method to display all the frame and its widgets
        '''
        # frame location
        self.frame.grid(row=4, column=0, rowspan=4, columnspan=3, padx=5, pady=5)
        if self.manual:
            # valve labels
            self.val1_label.grid(column=0,row=1)
            self.val2_label.grid(column=0,row=2)
            # on/off buttons
            self.btn_val1_on.grid(column=1,row=1, padx=0, pady=5)
            self.btn_val2_on.grid(column=1,row=2, padx=0, pady=0)
            self.btn_val1_off.grid(column=2, row=1, padx=5, pady=5)
            self.btn_val2_off.grid(column=2, row=2, padx=5, pady=0)
        else:
            # control label
            self.ctrl_label.grid(column=0,row=1)
            self.btn_off.grid(column=1,row=1, padx=5, pady=0)
            self.btn_pressurize.grid(column=1,row=2, padx=5, pady=5)
            self.btn_operation.grid(column=1,row=3, padx=5, pady=0)
        # save button
        self.btn_save_start.grid(column=3, row=1)
        self.btn_save_end.grid(column=3, row=2)

    def close(self):
        '''
        Method to close the controls GUI and destroys the widgets
        '''

        # Stop recording/close file if it is currently open
        if self.recording:
            self.ctrl_save_end()

        # destroy all the elements so they are not kept in system memory
        for widget in self.frame.winfo_children():
            widget.destroy()
        self.frame.destroy()
        self.root.geometry("372x130")

    def v1_OPEN(self):
        '''
        Method to turn valve 1 on
        '''  
        # update value
        self.v1 = 1

        # update GUI text
        self.btn_val1_on["state"] = "disable"
        self.btn_val1_off["state"] = "normal"

        # actuate pin
        self.serial.actuate("v1", "OPEN")

        # record valve states
        if self.recording:
            self.record_valve_states("Open Valve 1")

    def v2_OPEN(self):
        '''
        Method to turn valve 2 on
        '''  
        # update value
        self.v2 = 1

        # update GUI text
        self.btn_val2_on["state"] = "disable"
        self.btn_val2_off["state"] = "normal"
        
        # actuate pin
        self.serial.actuate("v2", "OPEN")

        if self.recording:
            self.record_valve_states("Open Valve 2")

    def v1_CLOSE(self):
        '''
        Method to turn valve 1 off
        '''    
        # update value
        self.v1 = 0

        # update GUI text
        self.btn_val1_on["state"] = "normal"
        self.btn_val1_off["state"] = "disable"

        # actuate pin
        self.serial.actuate("v1", "CLOSE")

        if self.recording:
            self.record_valve_states("Close Valve 1")

    def v2_CLOSE(self):
        '''
        Method to turn valve 2 off
        '''
        # update value
        self.v2 = 0

        # update GUI text
        self.btn_val2_on["state"] = "normal"
        self.btn_val2_off["state"] = "disable"

        # actuate pin
        self.serial.actuate("v2", "CLOSE")

        if self.recording:
            self.record_valve_states("Close Valve 2")

    def ctrl_off(self):
        '''
        Method to turn off whole control system
        '''
        # update values
        self.v1 = 0
        self.v2 = 0

        # update GUI text
        self.btn_off["state"] = "disable"
        self.btn_pressurize["state"] = "normal"
        self.btn_operation["state"] = "normal"

        # actuate pins
        self.serial.actuate("v2", "CLOSE")
        self.serial.actuate("v2", "CLOSE")

        if self.recording:
            self.record_valve_states("Turn off control system")
    
    def ctrl_pressurize(self):
        '''
        Method to pressurize system
        '''
        # update values
        self.v1 = 1
        self.v2 = 0

        # update GUI text
        self.btn_off["state"] = "normal"
        self.btn_pressurize["state"] = "disable"
        self.btn_operation["state"] = "normal"

        # actuate pins
        self.serial.actuate("v1", "OPEN")
        self.serial.actuate("v2", "CLOSE")

        if self.recording:
            self.record_valve_states("Pressurize system")
    
    def ctrl_operation(self):
        '''
        Method to go full operation
        '''
        # update values
        self.v1 = 1
        self.v2 = 1

        # update GUI text
        self.btn_off["state"] = "normal"
        self.btn_pressurize["state"] = "normal"
        self.btn_operation["state"] = "disable"

        # actuate pins
        self.serial.actuate("v1", "OPEN")
        self.serial.actuate("v2", "OPEN")
 
        if self.recording:
            self.record_valve_states("Enable full operation")

    def record_valve_states(self, command="N/A"):
        '''
        Method to write the current states of all valves into .csv, with global timestamp
        '''
        self.control_data.write(f"{time.time_ns()},{self.v1},{self.v2},{command}\n")

    def ctrl_save_start(self):
        '''
        Method to start saving control data into .csv
        '''       
        try:
            filename = "G:/Shared drives/TAMU Rocket Engine Design/02 Subsystems - General/04 AVI - Avionics & Controls/03 WTS/WTS GUI/Data/" + str(time.time_ns()) + "_control_data.csv"
            self.control_data = open(filename, 'w')
            self.recording = True

            # Header and initial state
            self.control_data.write("Global Time (ns from Epoch),Valve 1 State (1=Open),Valve 2 State(1=Open),Last Command\n")
            self.record_valve_states("Begin saving Data")

            # Update buttons + print for checking
            print("start save control data")
            self.btn_save_start["state"] = "disabled"
            self.btn_save_end["state"] = "normal"
        except:
            print("Cannot open control data file")

    def ctrl_save_end(self):
        '''
        Method to stop saving control data into .csv
        '''
        print("end save ctrl data")
        self.btn_save_start["state"] = "normal"
        self.btn_save_end["state"] = "disabled"
        self.recording = False
        try:
            self.control_data.close()
        except:
            print("Cannot close pressure data file")


# GUI for pressure data aquisition
class GUI_DAQ():
    def __init__(self, root, serial, port, control):
        '''
        Initialize the DAQ GUI and related widgets
        '''
        self.root = root
        self.serial = serial
        self.port = port
        self.control = control

        # Constants for mass flow rate
        self.Cd = 0.64165                         # coeff
        self.A = 3.14159/4.0*(0.125*0.0254)**2    # m^2 using A = pi*1/4(d**2), d is converted from inches to meters
        self.RHO = 998.0                          # kg/m^3

        # Recorded data
        self.pt1 = 0
        self.pt2 = 0
        self.mfr = 0

        # Tank volume
        self.vol_start = 1.0        # gal, initial volume
        self.vol = self.vol_start   # gal, remaining volume
        self.vol_perc = 100.0       # percentage, remaining volume
        
        # Data for plotting
        self.plt_time = []
        self.plt_pt1 = []
        self.plt_pt2 = []
        self.plt_mfr = []
        self.plt_pt1_mvg_avg = []
        self.plt_pt2_mvg_avg = []
        self.plt_mfr_mvg_avg = []

        # File
        self.pressure_data = None
        self.recording = False
        
        # frame
        self.frame = LabelFrame(root, text="Sensor Information", padx=5, pady=5, bg="#000", fg="#fff")
        # pressure & flow rate data labels
        self.pt1_label = Label(self.frame, text="Upstream Pressure (PU) [psia]: ", width=25, anchor="w", bg="#000", fg="#fff")
        self.pt2_label = Label(self.frame, text="Downstream Pressure (PD) [psia]: ", width=25, anchor="w", bg="#000", fg="#fff")
        self.mfr_label = Label(self.frame, text="Mass Flow Rate [kg/s]: ", width=25, anchor="w", bg="#000", fg="#fff")
        self.vol_label = Label(self.frame, text="Tank Volume [%]", width=25, anchor="w", bg="#000", fg="#fff")
        # pressure & flow rate data values
        self.pt1_current = Label(self.frame, text='-', width=10, anchor="e", bg="#000", fg="#fff")
        self.pt2_current = Label(self.frame, text='-', width=10, anchor="e", bg="#000", fg="#fff")
        self.mfr_current = Label(self.frame, text='-', width=10, anchor="e", bg="#000", fg="#fff")
        self.vol_current = Label(self.frame, text=f"{self.vol_perc:.5g}", width=10, anchor='e', bg='#000', fg="#fff")
        # save data button
        self.btn_save_start = Button(self.frame, text="Start Save", width=10,bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="normal", command=self.daq_save_start)
        self.btn_save_end = Button(self.frame, text="End Save", width=10,bg="#000", fg="#fff", highlightbackground="#000", highlightcolor="#fff", state="disabled", command=self.daq_save_end)          
        # Display all the frame and its widgets
        self.display()

        # Creates the graphs
        self.graphs = GUI_GRAPH(self.root, self.serial, self)

    def start(self):
        '''
        Method to read and update sensor data in real time
        '''
        # While the serial connection is online, reads data and updates boxes
        while self.serial.ser.status:
            self.read_data()
            self.update_text()

    def display(self):
        '''
        Method to display all the frame and its widgets
        '''
        # frame location
        self.frame.grid(row=8, column=0, rowspan=4, columnspan=3, padx=5, pady=5)
        # sensor data labels
        self.pt1_label.grid(column=0,row=1)
        self.pt2_label.grid(column=0,row=2)
        self.mfr_label.grid(column=0,row=3)
        self.vol_label.grid(column=0,row=4)
        # sensor data values
        self.pt1_current.grid(column=1,row=1, pady=5, padx=5)
        self.pt2_current.grid(column=1,row=2, pady=5, padx=5)
        self.mfr_current.grid(column=1,row=3, pady=5, padx=5)
        self.vol_current.grid(column=1,row=4, pady=5, padx=5)
        # button
        self.btn_save_start.grid(column=2, row=1)
        self.btn_save_end.grid(column=2, row=2)

    def close(self):
        '''
        Method to close the controls GUI and destroys the widgets
        '''
        # Close the graphs portion
        self.graphs.close()

        # Stop recording/close file if it is currently open
        if self.recording:
            self.daq_save_end()
        
        # Must destroy all the elements so they are not kept in Memory
        for widget in self.frame.winfo_children():
            widget.destroy()
        self.frame.destroy()

    def read_data(self):
        '''
        Method to record data from the teensy and save it locally for both the box display and graphing
        '''
        # read data from teensy
        try:
            local_time, p1, p2 = self.serial.read_sensors()
        except:
            #print("Too few values (or other error)")
            return 
        
        local_time = int(local_time) / 1000.0
        self.pt1 = float(p1)
        self.pt2 = float(p2)
        self.calculate_mfr()

        # writes data to file if currently recording sensor data
        if self.recording:
            self.pressure_data.write(f"{time.time_ns()},{local_time},{self.pt1},{self.pt2},{self.mfr},{self.vol_perc}\n")

        # Update the 20-point moving average
        self.plt_pt1_mvg_avg = self.plt_pt1_mvg_avg[-19:] + [self.pt1]
        self.plt_pt2_mvg_avg = self.plt_pt2_mvg_avg[-19:] + [self.pt2]
        self.plt_mfr_mvg_avg = self.plt_mfr_mvg_avg[-19:] + [self.mfr]

        # Update the plotting points (last 15 seconds at >25ms samples)
        if 20 <= len(self.plt_pt1_mvg_avg) and ((0 == len(self.plt_time)) or (local_time - self.plt_time[-1] > .025)):
            self.plt_time = self.plt_time[-500:] + [local_time]
            self.plt_pt1 = self.plt_pt1[-500:] + [sum(self.plt_pt1_mvg_avg) / 20.0]
            self.plt_pt2 = self.plt_pt2[-500:] + [sum(self.plt_pt2_mvg_avg) / 20.0]
            self.plt_mfr = self.plt_mfr[-500:] + [sum(self.plt_mfr_mvg_avg) / 20.0] 

            # update remaining volume using the average mfr (during operation) at >25 ms samples
            if self.control.btn_operation["state"] == "disabled":
                if self.vol >= 0.0:
                    self.vol -= (self.plt_mfr[-1]*.025)/self.RHO
                    self.vol_perc = float(self.vol/self.vol_start*100.0)
                else:
                    self.vol_perc = 0.0

    def update_text(self):
        '''
        Method to update sensor text
        '''
        if self.pt1:
            self.pt1_current.config(text=f"{self.pt1:.5g}")
            self.pt2_current.config(text=f"{self.pt2:.5g}")
            self.mfr_current.config(text=f"{self.mfr:.5g}")
        if self.control.btn_operation["state"] == "disabled":
            self.vol_current.config(text=f"{self.vol_perc:.5g}")
        self.frame.update()

    def calculate_mfr(self):
        '''
        Method to calculate mass flow rate
        '''
        # Convert strings of pressures to floats and convert from psi to pascal
        p1 = self.pt1 * 6894.75789
        p2 = self.pt2 * 6894.75789

        # Calculate mass flow rate, store as string
        self.mfr = self.Cd* self.A* (2*self.RHO*abs(p2 - p1)) ** 0.5

    def daq_save_start(self):
        '''
        Method to save pressure data into .csv
        '''
        try:
            filename = "G:/Shared drives/TAMU Rocket Engine Design/02 Subsystems - General/04 AVI - Avionics & Controls/03 WTS/WTS GUI/Data/" + str(time.time_ns()) + "_pressure_data.csv"
            self.pressure_data = open(filename, 'w')
            self.recording = True

            # Header
            self.pressure_data.write("Global Time (ns from Epoch),Local Time (ms from test),")
            self.pressure_data.write("Upsteam Pressure (psia),Downstream Pressure (psia),Mass Flow Rate (kg/s),Tank Volume (%)\n")

            # Update buttons + print for checking
            print("start save daq data")
            self.btn_save_start["state"] = "disabled"
            self.btn_save_end["state"] = "normal"
        except:
            print("Cannot open pressure data file")

    def daq_save_end(self):
        '''
        Method to stop saving pressure data into .csv
        '''
        print("end save daq data")
        self.btn_save_start["state"] = "normal"
        self.btn_save_end["state"] = "disabled"
        self.recording = False
        try:
            self.pressure_data.close()
        except:
            print("Cannot close pressure data file")

class GUI_GRAPH():
    def __init__(self, root, serial, daq):
        '''
        Initialize the DAQ GUI and related widgets
        '''
        self.root = root
        self.serial = serial
        self.daq = daq

        # frame
        self.frame = LabelFrame(root, text="Live Graphs", padx=5, pady=5, bg="#000", fg="#fff")

        # Graph parameters
        self.interval = 500    # ms between graph updates
        self.max_psi = 164.7

        # Plot layout
        self.fig, self.axs = plt.subplots(1, 2)
        self.fig.set_figheight(3.5)
        self.fig.set_figwidth(6.5)
        self.fig.tight_layout()
        self.fig.subplots_adjust(left=0.13, right=0.95, bottom=0.15, top=0.93, wspace=0.35, hspace=0.5)
        self.fig.patch.set_facecolor('black')
        self.params = {"ytick.color" : "w",
                       "xtick.color" : "w",
                       "axes.labelcolor" : "w",
                       "axes.edgecolor" : "w"}
        plt.rcParams.update(self.params)
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.frame)
        
                  
        # Display all the frame and its widgets
        self.display()

    def display(self):
        '''
        Method to display all the frame and its widgets
        '''
        # frame location
        self.frame.grid(row=1, column=4, rowspan=11, columnspan=2, padx=5, pady=5)

        # Plots
        self.canvas.get_tk_widget().grid(row=0, column=0, rowspan=11, columnspan=2, padx=2, pady=2)
        self.ani = FuncAnimation(self.fig, self.animate, interval=self.interval, cache_frame_data=False)

    def close(self):
        '''
        Method to close the controls GUI and destroys the widgets
        '''        
        # Must destroy all the elements so they are not kept in Memory
        for widget in self.frame.winfo_children():
            widget.destroy()
        self.frame.destroy()

    def animate(self, frame_data):
        """
        Method to call the update graphs function potentially multithreaded
        """
        self.update_graphs(frame_data)

        # Not sure if it will work; jittery and often doesn't render certain elements
        # x = threading.Thread(target=self.update_graphs, args=(self,))
        # x.start()

    def update_graphs(self, frame_data):
        """
        Method to update the graphs
        """
        # Domain values + Max Pressure
        self.x_vals = [t - self.daq.plt_time[0] for t in self.daq.plt_time]
        self.plt_max_psi = [self.max_psi] * len(self.x_vals)

        # Dual pressure plot
        ax = self.axs.flatten()[0]
        ax.clear()
        ax.plot(self.x_vals, self.daq.plt_pt1, linewidth=1, color='b', label="PU")
        ax.plot(self.x_vals, self.daq.plt_pt2, linewidth=1, color='g', label="PD")
        ax.plot(self.x_vals, self.plt_max_psi, linewidth=1, color='r', label="Danger")
        ax.set_xlabel("Time [s]", fontsize=10, color='w')
        ax.set_ylabel("Pressure [psia]", fontsize=10, color='w')
        ax.set_title("Pressure Readings (last 15 seconds)", fontsize=10, color='w')
        ax.legend(fontsize=7, bbox_to_anchor=(-0.4, 0), loc="upper left", framealpha=1, facecolor="black", labelcolor="white", edgecolor="black")
        ax.grid()
        if len(self.x_vals):
            bottom = min(min(self.daq.plt_pt1), min(self.daq.plt_pt2)) * .9
            top = max(max(self.daq.plt_pt1), max(self.daq.plt_pt2)) * 1.1
            ax.set_ylim(bottom, top)

        # Mass Flow Rate Plot
        ax = self.axs.flatten()[1]
        ax.clear()
        ax.plot(self.x_vals, self.daq.plt_mfr, linewidth=1, color='k', label="MFR")
        ax.set_xlabel("Time [s]", fontsize=10, color='w')
        ax.set_ylabel("Mass Flow Rate [kg/s]", fontsize=10, color='w')
        ax.set_title("Mass Flow Rate (last 15 seconds)", fontsize=10, color='w')
        ax.grid()
        if len(self.x_vals):
            bottom = min(self.daq.plt_mfr) * .9
            top = max(self.daq.plt_mfr) * 1.1
            ax.set_ylim(bottom, top)


if __name__ == "__main__":
    GUI_Root()
    GUI_COM()
    GUI_CTRL()
    GUI_DAQ()
    GUI_GRAPH()
