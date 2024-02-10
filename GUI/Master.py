# MASTER FILE; RUN THIS TO START GUI

# import GUI and communication scripts from other files
from GUI import GUI_Root, GUI_COM

# initiate the GUI classes
Root_Master = GUI_Root()
COM_Master = GUI_COM(Root_Master.root)

# start the GUI
Root_Master.root.mainloop()