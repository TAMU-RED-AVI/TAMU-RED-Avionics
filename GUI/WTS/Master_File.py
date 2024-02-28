# MASTER FILE; RUN THIS TO START GUI

# import GUI and communication scripts from other files
from GUI_Architecture import GUI_Root, GUI_COM
from Serial_Communication import Serial_COM

# initiate the GUI classes
Serial_Master = Serial_COM()
Root_Master = GUI_Root()

# initiate the commuication gui
COM_Master = GUI_COM(Root_Master.root, Serial_Master)

# start the GUI
Root_Master.root.mainloop()
