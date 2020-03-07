import sys

# for UI
from PyQt5 import QtCore, QtWidgets
from plot_window import PlotWindow


class Controller:

    def show_plot_window(self):
        self.plot_window = QtWidgets.QMainWindow()
        self.ui = PlotWindow()
        self.ui.setup_ui(self.plot_window)
        self.plot_window.show()
        app.aboutToQuit.connect(self.close_threads)


    def close_threads(self):
        self.ui.close_threads()


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    controller = Controller()
    controller.show_plot_window()
    sys.exit(app.exec_())
