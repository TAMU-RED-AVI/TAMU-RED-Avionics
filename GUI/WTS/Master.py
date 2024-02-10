from GUI_Master import RootGUI, ComGUI
from Serial_Com_ctrl import SerialCtrl

MySerial = SerialCtrl()
RootMaster = RootGUI()

ComMaster = ComGUI(RootMaster.root, MySerial)

RootMaster.root.mainloop()
