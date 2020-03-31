# ===================================
#   Name: Edward (Eddie) Guo
#   ID: 1576381
#   Partner: Jason Kim
#   CMPUT 275, Fall 2020
#
#   Final Assignment: EEG Visualizer
# ===================================

"""
Contains the QApplication which holds the PlotWindow QMainWindow object. The
controller class is here for convenient additions of extra QMainWindows.
"""

import sys

# for UI
from PyQt5 import QtCore, QtWidgets
from plot_window import PlotWindow


class Controller:

    """Controller class for slave QMainWindows. Used for expandability in case
    the user wishes to create additional windows for the program (ex: home
    window).
    """

    def show_plot_window(self):
        """Creates the main window (EEG and FFT plots) from plot_window.py.
        """
        self.plot_window = QtWidgets.QMainWindow()
        self.ui = PlotWindow()
        self.ui.setup_ui(self.plot_window)
        self.plot_window.setWindowFlags(QtCore.Qt.Window)
        self.plot_window.show()
        app.aboutToQuit.connect(self.close_threads)


    def close_threads(self):
        """Helper function that closes all running threads when the application
        is about to quit.
        """
        self.ui.close_threads()


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    controller = Controller()
    controller.show_plot_window()
    sys.exit(app.exec_())
