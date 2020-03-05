import sys
import random
import worker_threads as wt
from collections import deque

# for data streaming
import pylsl

# for UI
from PyQt5 import QtCore, QtWidgets
from mpl_canvas import MplCanvas

# for data plotting
import matplotlib.pyplot as plt
plt.style.use('seaborn')


class MainWindow(QtWidgets.QMainWindow):

    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)

        random.seed(0)
        self.setup_ui()

        n_data = 100
        self.xdata = deque([-(n_data/10 - i/10) for i in range(n_data)])
        self.ydata = deque([random.uniform(-64, 64) for i in range(n_data)])

        # Storing a reference to the plotted line
        self._plot_ref = None
        self.show()

        # multithreading
        self.threadpool = QtCore.QThreadPool()

        # Setup a timer to trigger the redraw
        self.timer = QtCore.QTimer()
        self.timer.setInterval(100)
        self.timer.timeout.connect(self.eeg_worker)
        self.timer.start()


    def setup_ui(self):
        self.setStyleSheet(
            'QMainWindow {'
            '   color: white;'
            '   background-color: white;'
            '}'
        )
        # main window
        self.setWindowTitle('EEG Visualizer')
        self._main = QtWidgets.QWidget()
        # eeg and fft windows
        self.setup_eeg_window()
        self.setup_fft_window()

        # layout
        layout = QtWidgets.QGridLayout(self._main)
        self.setCentralWidget(self._main)
        layout.addWidget(self.eeg_canvas, 0, 0)
        layout.addWidget(self.fft_canvas, 0, 1)


    def setup_eeg_window(self):
        self.eeg_canvas = MplCanvas(self, width=9, height=3, dpi=100)
        self.eeg_canvas.axes.set_ylim(-150, 150)
        self.eeg_canvas.axes.set_title('Raw Time Series')
        self.eeg_canvas.axes.set_xlabel('Time (s)')
        self.eeg_canvas.axes.get_yaxis().set_visible(False)
        self.eeg_canvas.fig.tight_layout()


    def setup_fft_window(self):
        self.fft_canvas = MplCanvas(self, width=6, height=3, dpi=100)
        self.fft_canvas.axes.set_title('FFT Plot')
        self.fft_canvas.axes.set_xlabel('Frequency (Hz)')
        self.fft_canvas.axes.get_yaxis().set_visible(False)
        self.fft_canvas.fig.tight_layout()


    def update_eegplot(self):
        self.ydata.popleft()
        self.ydata.append(random.uniform(-64, 64))

        if self._plot_ref is None:
            self._plot_ref, = self.eeg_canvas.axes.plot(self.xdata,
                self.ydata, 'k')
        else:
            self._plot_ref.set_ydata(self.ydata)

        # trigger the canvas to update and redraw
        self.eeg_canvas.draw()

    def eeg_worker(self):
        worker = wt.Worker(self.update_eegplot)
        self.threadpool.start(worker)


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    w = MainWindow()
    sys.exit(app.exec_())
