import random, time, ctypes, threading
import worker_threads as wt
import fft as my_fft

# for data streaming
from pylsl import StreamInlet, resolve_stream

# for UI
from PyQt5 import QtCore, QtWidgets
from mpl_canvas import MplCanvas

# for data plotting
import numpy as np
import matplotlib.pyplot as plt
plt.style.use('dark_background')


class PlotWindow():
    # number of points to plot
    n_data = 1000
    # timepoint at which to plot
    iter_n = 0
    sampling_rate = 100

    # window setting constants
    ymin, ymax = -64, 64
    ylim_min, ylim_max = -150, 150
    x_time = 10

    # initial settings
    resolved = False

    def setup_ui(self, MainWindow):
        self._run_thread = True

        MainWindow.setWindowTitle('EEG Visualizer')
        MainWindow.setStyleSheet(
            'QMainWindow {'
            '   color: black;'
            '   background-color: black;'
            '}'
        )

        self.central_widget = QtWidgets.QWidget(MainWindow)
        MainWindow.setCentralWidget(self.central_widget)
        self.central_widget.setStyleSheet(
            'QMainWindow {'
            '   color: black;'
            '   background-color: black;'
            '}'
        )

        self.setup_widgets()

        # raw data to analyze
        self.xdata = [i/100 for i in range(self.n_data)]
        self.ydata = [0 for i in range(self.n_data)]

        # multithreading
        self.threadpool = QtCore.QThreadPool()


    def setup_widgets(self):
        # creating eeg and fft windows
        self.central_widget.setWindowTitle('EEG Visualizer')
        self.setup_eeg_window()
        self.setup_fft_window()
        # organizing main window geometry
        layout = QtWidgets.QGridLayout(self.central_widget)
        layout.addWidget(self.eeg_canvas, 0, 0)
        layout.addWidget(self.fft_canvas, 0, 1)
        # getting streams button
        get_stream_btn = QtWidgets.QPushButton('Get Streams')
        get_stream_btn.setStyleSheet(
            'QPushButton {'
            '   color: white;'
            '   background-color: #282C34;'
            '   font-family: Helvetica;'
            '   max-height: 25px;'
            '   max-width: 125px;'
            '   text-align: center;'
            '}'
        )
        # can only get the data once
        self.get_stream_clicked = True
        get_stream_btn.clicked.connect(self.get_stream)
        layout.addWidget(get_stream_btn, 1, 0, alignment=QtCore.Qt.AlignCenter)


    def get_stream(self):
        if self.get_stream_clicked:
            self.start_thread(self.pull_data)
            self.get_stream_clicked = False
            # 1 second timeout
            self.kill_data_pull(timeout=1)


    def kill_data_pull(self, timeout):
        time.sleep(timeout)
        if self.resolved is False:
            self.get_stream_clicked = True
            self.show_popup_msg()
        else:
            self.start_thread(self.update_eegplot)
            self.start_thread(self.update_fftplot)


    def show_popup_msg(self):
        no_data_msg = QtWidgets.QMessageBox()
        no_data_msg.setWindowTitle('Error')
        no_data_msg.setText('No channels detected.')

        no_data_msg.setIcon(QtWidgets.QMessageBox.Critical)
        no_data_msg.setDefaultButton(QtWidgets.QMessageBox.Ok)
        no_data_msg.setDetailedText('Ensure that EEG data is streaming over'
            ' the correct network.')
        no_data_msg.exec_()


    def setup_eeg_window(self):
        self.eeg_canvas = MplCanvas(self, width=9, height=3, dpi=100)
        self.eeg_canvas.axes.set_ylim(self.ylim_min, self.ylim_max)
        self.eeg_canvas.axes.set_title('EEG')
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
        # storing a reference to the plotted lines for the eeg plot
        plot_ref = None
        moving_ref = None

        # update the plot
        while self._run_thread:
            if plot_ref is None:
                plot_ref, = self.eeg_canvas.axes.plot(self.xdata,
                    self.ydata, '#00FF7F', linewidth=1)
            else:
                plot_ref.set_ydata(self.ydata)

            if moving_ref is not None:
                moving_ref.remove()
            moving_ref = self.eeg_canvas.axes.axvline(
                self.xdata[self.iter_n], self.ymin, self.ymax, c='r')

            # trigger the canvas to update and redraw
            self.eeg_canvas.draw()
            time.sleep(0.01)


    def update_fftplot(self):
        # sampling frequency
        st = 1.0 / self.sampling_rate

        # getting number of frequency bins
        bins = np.fft.fftfreq(self.n_data, st)
        plot_ref = None

        while self._run_thread:
            fft = my_fft.transform(np.asarray(self.ydata), False)
            if plot_ref is not None:
                plot_ref.remove()
            plot_ref, = self.fft_canvas.axes.plot(bins[:(self.n_data >> 1)],
                np.abs(fft[:(self.n_data >> 1)]), '#00FF7F', linewidth=1)

            self.fft_canvas.draw()
            # TODO: slider of the fft frequency
            time.sleep(0.1)


    def pull_data(self):
        print('Looking for an EEG stream...')
        streams = resolve_stream('type', 'EEG')
        inlet = StreamInlet(streams[0])

        self.resolved = True
        while self._run_thread:
            sample, timestamp = inlet.pull_sample()
            self.ydata[self.iter_n] = sample[0]
            self.iter_n = (self.iter_n + 1) % self.n_data


    def start_thread(self, *args):
        for func in args:
            self.thread = wt.Worker(func)
            self.threadpool.start(self.thread)


    def close_threads(self):
        self._run_thread = False
        self.threadpool.waitForDone()
