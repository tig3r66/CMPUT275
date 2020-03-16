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
    sampling_rate, n_data = 100, 1000
    # timepoint at which to plot
    iter_n = 0

    # window setting constants
    ymin, ymax = -64, 64
    ylim_min, ylim_max = -150, 150
    xlim_min, xlim_max = 0, 10
    x_time = 10

    # initial settings
    resolved = False

    def setup_ui(self, MainWindow):
        self.setup_mainwindow(MainWindow)
        self.setup_widgets()

        # raw data to analyze
        self.xdata = [i/100 for i in range(self.n_data)]
        self.ydata = [0 for i in range(self.n_data)]
        # fft data
        self.fft = []
        # sampling frequency
        self.st = 1.0 / self.sampling_rate
        # getting number of frequency bins
        self.bins = np.fft.fftfreq(self.n_data, self.st)

        # multithreading
        self.threadpool = QtCore.QThreadPool()
        self._run_thread = True


    def setup_mainwindow(self, MainWindow):
        MainWindow.resize(1400, 400)
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
            '   color: white;'
            '   background-color: black;'
            '}'
        )
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setStyleSheet(
            'QStatusBar {'
            '   color: #393f4d;'
            '   background-color: #393f4d'
            '}'
        )
        self.sampling_freq_lbl = QtWidgets.QLabel('Sampling rate:')
        self.sampling_freq_lbl.setStyleSheet(
            'QLabel {'
            '   color: #feda6a;'
            '}'
        )
        self.statusbar.addWidget(self.sampling_freq_lbl)
        MainWindow.setStatusBar(self.statusbar)


    def setup_widgets(self):
        # creating eeg and fft windows
        self.central_widget.setWindowTitle('EEG Visualizer')
        self.setup_eeg_window()
        self.setup_fft_window()

        # organizing main window geometry
        self.master_layout = QtWidgets.QVBoxLayout(self.central_widget)
        self.layout = QtWidgets.QGridLayout()
        self.layout.addWidget(self.eeg_canvas, 0, 0)
        self.layout.addWidget(self.fft_canvas, 0, 1)
        top_widget = QtWidgets.QWidget()
        top_widget.setLayout(self.layout)
        self.master_layout.addWidget(top_widget)

        bottom_layout = QtWidgets.QHBoxLayout()
        # left spacer
        left_spacer = QtWidgets.QWidget()
        bottom_layout.addWidget(left_spacer)

        # getting streams button
        get_stream_btn = QtWidgets.QPushButton('Get Streams')
        get_stream_btn.setStyleSheet(
            'QPushButton {'
            '   color: black;'
            '   background-color: #feda6a;'
            '   font-family: Helvetica;'
            '   max-height: 200px;'
            '   max-width: 150px;'
            '   text-align: center;'
            '}'
        )
        # can only get the data once
        self.get_stream_clicked = True
        get_stream_btn.clicked.connect(self.get_stream)
        bottom_layout.addWidget(get_stream_btn)

        # stopping streams button
        stop_streams_btn = QtWidgets.QPushButton('Stop Streams')
        stop_streams_btn.setStyleSheet(
            'QPushButton {'
            '   color: black;'
            '   background-color: #feda6a;'
            '   font-family: Helvetica;'
            '   max-height: 200px;'
            '   max-width: 150px;'
            '   text-align: center;'
            '}'
        )
        stop_streams_btn.clicked.connect(self.stop_streams)
        bottom_layout.addWidget(stop_streams_btn)

        # right spacer
        right_spacer = QtWidgets.QWidget()
        bottom_layout.addWidget(right_spacer)

        # placing layout into a placeholder widget
        bottom_widget = QtWidgets.QWidget()
        bottom_widget.setLayout(bottom_layout)
        # bottom to master
        self.master_layout.addWidget(bottom_widget)


    def stop_streams(self):
        self.close_threads()
        # resetting
        self._run_thread = True
        self.get_stream_clicked = True
        self.eeg_plot_ref.remove()
        self.fft_plot_ref.remove()
        self.moving_ref.remove()
        self.eeg_canvas.draw()
        self.fft_canvas.draw()
        self.sampling_freq_lbl.setText('Sampling rate:')
        self.reset_data()


    def reset_data(self):
        self.iter_n = 0
        # raw data to analyze
        self.xdata = [i/100 for i in range(self.n_data)]
        self.ydata = [0 for i in range(self.n_data)]
        # fft data
        self.fft = []


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
            self.sampling_freq_lbl.setText('Sampling rate: {:.1f} Hz'.\
                format(self.sampling_rate))
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
        self.eeg_canvas = MplCanvas(self, width=6, height=3, dpi=100)
        self.eeg_canvas.axes.set_xlim(self.xlim_min, self.xlim_max)
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
        self.eeg_plot_ref = None
        self.moving_ref = None

        # update the plot
        while self._run_thread:
            if self.eeg_plot_ref is None:
                self.eeg_plot_ref, = self.eeg_canvas.axes.plot(self.xdata,
                    self.ydata, '#00FF7F', linewidth=1)
            else:
                self.eeg_plot_ref.set_ydata(self.ydata)

            if self.moving_ref is not None:
                self.moving_ref.remove()
            self.moving_ref = self.eeg_canvas.axes.axvline(
                self.xdata[self.iter_n], self.ymin, self.ymax, c='r')

            # trigger the canvas to update and redraw
            self.eeg_canvas.draw()
            time.sleep(0.02)


    def update_fftplot(self):
        self.fft_plot_ref = None

        while self._run_thread:
            self.fft = my_fft.transform(np.asarray(self.ydata), False)
            if self.fft_plot_ref is not None:
                self.fft_plot_ref.remove()
            self.fft_plot_ref, = self.fft_canvas.axes.plot(
                self.bins[:(self.n_data >> 1)],
                np.abs(self.fft[:(self.n_data >> 1)]), '#00FF7F', linewidth=1)

            self.fft_canvas.draw()
            # TODO: slider of the fft frequency
            time.sleep(0.1)


    def pull_data(self):
        print('Looking for an EEG stream...')
        streams = resolve_stream('type', 'EEG')
        inlet = StreamInlet(streams[0])
        # stream has been found
        self.resolved = True
        # setting stream info
        stream_info = inlet.info()
        # setting class attributes
        self.sampling_rate = stream_info.nominal_srate()
        self.n_data = int(self.sampling_rate * self.x_time)

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
