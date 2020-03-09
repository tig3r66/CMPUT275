import random
import worker_threads as wt
from collections import deque

# for data streaming
from pylsl import StreamInlet, resolve_stream

# for UI
from PyQt5 import QtCore, QtWidgets
from mpl_canvas import MplCanvas

# for data plotting
import matplotlib.pyplot as plt
plt.style.use('dark_background')


class PlotWindow():
    n_data = 100
    iter_n = 0

    def setup_ui(self, MainWindow):
        self._run_thread = True

        MainWindow.setStyleSheet(
            'QMainWindow {'
            '   color: white;'
            '   background-color: white;'
            '}'
        )

        self.central_widget = QtWidgets.QWidget(MainWindow)
        MainWindow.setCentralWidget(self.central_widget)
        self.central_widget.setStyleSheet(
            'QMainWindow {'
            '   color: white;'
            '   background-color: white;'
            '}'
        )

        # creating eeg and fft windows
        self.central_widget.setWindowTitle('EEG Visualizer')
        self.setup_eeg_window()
        self.setup_fft_window()
        # organizing main window geometry
        layout = QtWidgets.QGridLayout(self.central_widget)
        layout.addWidget(self.eeg_canvas, 0, 0)
        layout.addWidget(self.fft_canvas, 0, 1)

        # setting up data streams
        random.seed(0)

        self.xdata = deque([i/10 for i in range(self.n_data)])
        self.ydata = deque([random.uniform(-64, 64) for i in range(self.n_data)])

        # Storing a reference to the plotted line
        self._plot_ref = None
        self._moving_ref = None

        # multithreading
        self.threadpool = QtCore.QThreadPool()
        self.start_thread(self.pull_data)

        # Setup a timer to trigger the redraw
        self.timer = QtCore.QTimer()
        self.timer.setInterval(100)
        self.timer.timeout.connect(lambda: self.start_thread(self.update_eegplot))
        self.timer.start()


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
        self.ydata[self.iter_n] = random.uniform(-64, 64)
        self.iter_n = (self.iter_n + 1) % self.n_data

        if self._plot_ref is None:
            self._plot_ref, = self.eeg_canvas.axes.plot(self.xdata,
                self.ydata, '#00FF7F', linewidth=1)
        else:
            self._plot_ref.set_ydata(self.ydata)

        if self._moving_ref != None:
            self._moving_ref.remove()
        self._moving_ref = self.eeg_canvas.axes.axvline(
            self.xdata[self.iter_n], -64, 64, c='r')

        # trigger the canvas to update and redraw
        self.eeg_canvas.draw()


    def pull_data(self):
        # TODO: make this update a buffer to plot from
        print('Looking for an EEG stream...')
        streams = resolve_stream('type', 'EEG')
        inlet = StreamInlet(streams[0])
        while self._run_thread:
            sample, timestamp = inlet.pull_sample()
            # print('timestamp:', timestamp, 'sample:', sample)


    def start_thread(self, *args):
        for func in args:
            self.thread = wt.Worker(func)
            self.threadpool.start(self.thread)


    def close_threads(self):
        self._run_thread = False
        self.threadpool.waitForDone()
