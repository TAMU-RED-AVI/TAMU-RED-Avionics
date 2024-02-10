# imports
from tkinter import *
from tkinter import messagebox


# GUI root
class RootGUI():
    # GUI root initialization function
    def __init__(self):
        self.root = Tk()                                                            # GUI root using tkinter
        self.root.title("Serial Communication")                                     # tile
        self.root.geometry("360x120")                                               # window size
        self.root.config(bg="white")                                                # window backround color

# communication frame
class ComGUI():

    # coms initialization function
    def __init__(self, root, serial):
        self.root = root                                                            # GUI root
        self.serial = serial                                                        # serial object

        self.padx = 20                                                              # x padding var
        self.pady = 5                                                               # y padding var
        self.frame = LabelFrame(root, text="Coms Manager",                            # coms frame
                                bg="white", padx=5, pady=5)                     
        self.label_ap = Label(self.frame, text="Available Port(s): ",               # available ports label
                              bg="white", width=15, anchor="w")
        self.label_bd = Label(self.frame, text="Baude Rate: ",                      # baude rate label
                              bg="white", width=15, anchor="w")
        self.ComOptionMenu()                                                        # call coms dropdown menu
        self.BaudeOptionMenu()                                                      # call bds dropdown menu
        self.btn_refresh = Button(self.frame, text="Refresh",                       # refresh coms button
                                  width=10, command=self.com_refresh)
        self.btn_connect = Button(self.frame, text="Connect", width=10,             # connect serial button
                                  command=self.serial_connect, state="disabled")
        self.publish()                                                              # call placements

    # coms options function
    def ComOptionMenu(self):
        self.serial.getCOMList()                                                    # communication options
        self.clicked_com = StringVar()                                              # storing clicked option
        self.clicked_com.set(self.serial.com_list[0])                               # default option is "-"
        self.drop_com = OptionMenu(self.frame, self.clicked_com,                    # com dropdown menu
                                   *self.serial.com_list, command=self.connect_ctrl)
        self.drop_com.config(width=10)                                              # com dropdown menu size

    # baude rate options function
    def BaudeOptionMenu(self):
        bds = ["-", "300", "600", "1200", "1200", "2400", "4800"]                   # baude rate options placeholder
        self.clicked_bd = StringVar()                                               # storing clicked option
        self.clicked_bd.set(bds[0])                                                 # default option is "-"
        self.drop_bd = OptionMenu(self.frame, self.clicked_bd,                      # bd dropdown menu 
                                  *bds, command=self.connect_ctrl)
        self.drop_bd.config(width=10)                                               # bd dropdown menu size

    # dropdown menus logic
    def connect_ctrl(self, other):
        if "-" in self.clicked_com.get() or "-" in self.clicked_bd.get():           # keeps "connect" button disabled when "-" is selected
            self.btn_connect["state"] = "disable"
        else:
            self.btn_connect["state"] = "active"
    
    # refresh button logic
    def com_refresh(self):
        self.drop_com.destroy()
        self.ComOptionMenu()
        self.drop_com.grid(column=2, row=2, 
                           padx=self.padx, pady=self.pady)                                 # com dropdown menu placement
        logic = []
        self.connect_ctrl(logic)

    # serial connect button logic
    def serial_connect(self):
        if self.btn_connect["text"] in "Connect":
            self.serial.SerialOpen(self)
            if self.serial.ser.status:
                self.btn_connect["text"] = "Disconnect"
                self.btn_refresh["state"] = "disable"
                self.drop_bd["state"] = "disable"
                self.drop_com["state"] = "disable"
                InfoMsg = f"Successful UART connection using {self.clicked_com.get()}"
                messagebox.showinfo("showinfo", InfoMsg)
                self.conn = ConnGUI(self.root, self.serial)
            else:
                ErrorMsg = f"Failure to establish UART connection using {self.clicled.com.get()} "
                messagebox.showerror("showerror", ErrorMsg)
        else:
            #start closing connection
            self.serial.SerialClose()
            InfoMsg = f"UART connection using {self.clicked_com.get()} is now closed"
            messagebox.showwarning("showinfo", InfoMsg)
            self.btn_connect["text"] = "Connect"
            self.btn_refresh["state"] = "active"
            self.drop_bd["state"] = "active"
            self.drop_com["state"] = "active"
    
    # placement function
    def publish(self):                                               
        self.frame.grid(row=0,column=0, rowspan=3, 
                        columnspan=3, padx=5, pady=5)                            # frame placement

        self.label_ap.grid(column=1, row=2)                                                                 # available ports placement
        self.drop_com.grid(column=2, row=2, 
                           padx=self.padx, pady=self.pady)                                 # com dropdown menu placement
        self.label_bd.grid(column=1, row=3)                                                                 # baude rate placement
        self.drop_bd.grid(column=2, row=3)                                                                  # baude rate dropdown menu placement
        
        self.btn_refresh.grid(column=3, row=2)                                                              # refresh button placement
        self.btn_connect.grid(column=3, row=3)                                                              # connect button placement

class ConnGUI():
    def __init__(self, root, serial):
        self.root = root
        self.serial = serial

        self.frame = LabelFrame(root, text="Connection Manager", 
                                padx=5, pady=5, bg="white", width=60)
        self.sync_label = Label(
            self.frame, text="Sync Status: ", bg="white", width=15, anchor="w")
        self.sync_status = Label(self.frame, text="..Sync..", bg="white", fg="orange", width=5)
        self.ch_label = Label(self.frame, text="Active Channels: ", bg="white", width=15, anchor="w")
        self.ch_status = Label(self.frame, text="...", bg="white", fg="orange", width=5)
        
        self.ConnGUIOpen()

        


    def ConnGUIOpen(self):
        self.root.geometry("800x120")
        self.frame.grid(row=0,column=4,rowspan=3,columnspan=5,padx=5,pady=5)
        self.sync_label.grid(column=1, row=1)
        self.sync_status.grid(column=2, row=1)

        self.ch_label.grid(column=1, row=2)
        self.ch_status.grid(column=2, row=2, pady=self.pady)


# excecutes when in main
if __name__ == "__main__":
    RootGUI()
    ComGUI()
    ConnGUI()
