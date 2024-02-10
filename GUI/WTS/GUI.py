# GUI ARCHITECTURE

from tkinter import *

class GUI_Root():
    def __init__(self):
        '''
        Initiallizing the root GUI
        '''
        self.root = Tk()
        self.root.title("Water Test Stand GUI")
        self.root.geometry("360x120")
        self.root.config(bg="white")

class GUI_COM():
    def __init__(self, root):
        '''
        Initialize the connection GUI and related widgets
        '''
        # frame
        self.root = root
        self.frame = LabelFrame(root, text="Teensy Communication", padx=5, pady=5, bg="white")
        
        # widget labels
        self.port_label = Label(self.frame, text="Comm Port: ", bg="white", width=15, anchor="w")
        self.baud_label = Label(self.frame, text="Baud Rate: ", bg="white", width=15, anchor="w")

        self.port = Label(self.frame, text="none", bg="white", width=15, anchor="w")
        self.baud = Label(self.frame, text="none", bg="white", width=15, anchor="w")

        # Display all the frame and its widgets
        self.display()
    
    def display(self):
        '''
        Method to display all the frame and its widgets
        '''
        self.frame.grid(row=0, column=0,
                        rowspan=3, columnspan=3,
                        padx=5, pady=5)
        
        self.port_label.grid(column=1,row=1)
        self.baud_label.grid(column=1,row=2)

        self.port.grid(column=2,row=1)
        self.baud.grid(column=2,row=2)
