# ===================================
#   Name: Edward (Eddie) Guo
#   ID: 1576381
#   Partner: Jason Kim
#   CMPUT 275, Fall 2020
#
#   Final Assignment: EEG Visualizer
# ===================================

"""
Matplotlib backend to create a QWidget that can be embedded in a QMainWindow.
This is used for the live EEG and FFT plots in plot_window.py.
"""

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

class MplCanvas(FigureCanvas):

    """Creates a figure and adds a single set of axes. The canvas object is a
    QWidget that can be embedded into Qt.
    """

    def __init__(self, parent=None, width=10, height=5, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(111)
        super(MplCanvas, self).__init__(self.fig)
