# ===================================
#   Name: Edward (Eddie) Guo
#   ID: 1576381
#   Partner: Jason Kim
#   CMPUT 275, Fall 2020
#
#   Final Assignment: EEG Visualizer
# ===================================

"""
Plot window class for instantiation by a QMainWindow. PlotWindow plots the live
EEG and FFT plots, and it includes helper methods for multi-threading and data
buffer management.
"""

import time, ctypes, threading
import helper_classes.worker_threads as wt

# for data streaming
from pylsl import StreamInlet, resolve_stream

# for UI
from PyQt5 import QtCore, QtWidgets
from helper_classes.mpl_canvas import MplCanvas

# for data plotting
import fft as my_fft
import numpy as np
import matplotlib.pyplot as plt
plt.style.use('dark_background')


class PlotWindow():

    """Creates a subwindow including the live EEG data and FFT plots. This acts
    as the UI for the QMainWindow found in main.py.
    """

    # default number of points to plot (updated when data streams are pulled)
    sampling_rate, n_data = 100, 1000
    # timepoint at which to plot
    iter_n = 0

    # window setting constants
    ymin, ymax = -64, 64
    ylim_min, ylim_max = -150, 150
    xlim_min, xlim_max = 0, 10
    x_time = 10

    # determines whether to continue or stop threads
    resolved = False

    def setup_ui(self, MainWindow):
        """Initializes the plot windows and threadpool for use in the
        QMainWindow. Must be called when instantiating the QMainWindow.

        Arguments:
            MainWindow (QMainWindow): the object to instantiate with the
                PlotWindow UI.
        """
        self.setup_mainwindow(MainWindow)
        self.setup_widgets()

        # raw data to analyze
        # the size of these initial settings are updated when data is pulled
        self.xdata = [i/100 for i in range(self.n_data)]
        self.ydata = [0 for i in range(self.n_data)]
        # fft data
        self.fft = []
        # sampling frequency (updated when data is pulled)
        self.st = 1.0 / self.sampling_rate
        # number of frequency bins (updated when data is pulled)
        self.bins = np.fft.fftfreq(self.n_data, self.st)

        # 3 major threads: data pull, EEG plot, FFT plot
        self.threadpool = QtCore.QThreadPool()
        self._run_thread = True


    def setup_mainwindow(self, MainWindow):
        """Sets up the QMainWindow dimensions, statusbar, title, etc.

        Arguments:
            MainWindow (QMainWindow): the object to instantiate with the
                PlotWindow UI.
        """
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
        """Creates the layout for the main window. Consists of a master layout
        (QVBoxLayout) which holds two child layouts. Each child layout holds
        their respective widgets.
        """
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
        """Stops the major threads, resets the sampling rate label, and clears
        the plots when the button pressed signal is emitted.
        """
        try:
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
        except AttributeError:
            # the plot references do not exist yet, so do nothing
            pass
        except ValueError:
            # cannot remove an empty list
            pass


    def reset_data(self):
        """Helper function which resets all sources of plotted data when the
        streams are stopped.
        """
        self.iter_n = 0
        # raw data to analyze
        self.xdata = [i/100 for i in range(self.n_data)]
        self.ydata = [0 for i in range(self.n_data)]
        # fft data
        self.fft = []


    def get_stream(self):
        """Attempts to start the streams when the start streams button is
        pressed (this function is connected to the button pressed signal). The
        timeout is 1 second before an error dialog pops up.
        """
        if self.get_stream_clicked:
            self.start_thread(self.pull_data)
            self.get_stream_clicked = False
            # 1 second timeout
            self.kill_data_pull(timeout=1)


    def kill_data_pull(self, timeout):
        """Helper function for get_stream. If the data pulling is not successful
        after the timeout, an error dialog pops up. Otherwise, it starts the
        relevant threads needed for plotting.

        Arguments:
            timeout (int, float, double): the timeout in seconds.
        """
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
        """A helper function which pops an error dialog if no EEG channels
        are detected on the local network.
        """
        no_data_msg = QtWidgets.QMessageBox()
        no_data_msg.setWindowTitle('Error')
        no_data_msg.setText('No channels detected.')

        no_data_msg.setIcon(QtWidgets.QMessageBox.Critical)
        no_data_msg.setDefaultButton(QtWidgets.QMessageBox.Ok)
        no_data_msg.setDetailedText('Ensure that EEG data is streaming over'
            ' the correct network.')
        no_data_msg.exec_()


    def setup_eeg_window(self):
        """Sets the layout of the EEG plot window.
        """
        self.eeg_canvas = MplCanvas(self, width=6, height=3, dpi=100)
        self.eeg_canvas.axes.set_xlim(self.xlim_min, self.xlim_max)
        self.eeg_canvas.axes.set_ylim(self.ylim_min, self.ylim_max)
        self.eeg_canvas.axes.set_title('EEG')
        self.eeg_canvas.axes.set_xlabel('Time (s)')
        self.eeg_canvas.axes.get_yaxis().set_visible(False)
        self.eeg_canvas.fig.tight_layout()


    def setup_fft_window(self):
        """Sets the layout of the FFT plot window.
        """
        self.fft_canvas = MplCanvas(self, width=6, height=3, dpi=100)
        self.fft_canvas.axes.set_title('FFT Plot')
        self.fft_canvas.axes.set_xlabel('Frequency (Hz)')
        self.fft_canvas.axes.get_yaxis().set_visible(False)
        self.fft_canvas.fig.tight_layout()


    def update_eegplot(self):
        """Continually plots a live EEG graph with a moving vertical line
        indicating the most recent timepoint pulled.
        """
        # storing a reference to the plotted lines for the eeg plot
        self.eeg_plot_ref = None
        self.moving_ref = None

        # update the plot
        while self._run_thread:
            # updating the EEG signals
            if self.eeg_plot_ref is None:
                self.eeg_plot_ref, = self.eeg_canvas.axes.plot(self.xdata,
                    self.ydata, '#00FF7F', linewidth=1)
            else:
                self.eeg_plot_ref.set_ydata(self.ydata)

            # updating the vertical line
            if self.moving_ref is not None:
                self.moving_ref.remove()
            self.moving_ref = self.eeg_canvas.axes.axvline(
                self.xdata[self.iter_n], self.ymin, self.ymax, c='r')

            # trigger the canvas to update and redraw
            self.eeg_canvas.draw()
            time.sleep(0.02)


    def update_fftplot(self):
        """Continually plots the FFT (absolute value of the complex output) of
        the EEG graph. Takes half the FFT plot due to symmetry and the Nyquist
        sampling limit.
        """
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
        """Continually pulls data from an LSL stream over the local network and
        updates data sampling rate-related class attributes.
        """
        print('Looking for an EEG stream...')
        streams = resolve_stream('type', 'EEG')
        inlet = StreamInlet(streams[0])
        # stream has been found
        self.resolved = True
        # getting stream metadata
        stream_info = inlet.info()
        # setting class attributes
        self.sampling_rate = stream_info.nominal_srate()

        # printing stream info
        print(f"{'':=^30s} Streams {'':=^30s}")
        print(f'Channel name: {stream_info.name()}'
              f'\nData type: {stream_info.type()}'
              f'\nChannel count: {stream_info.channel_count()}'
              f'\nSource ID: {stream_info.source_id()}'
            )
        # nice ;)
        print(f"{'':=^69s}")

        # updating sampling rate
        self.n_data = int(self.sampling_rate * self.x_time)
        while self._run_thread:
            # timestamp unused
            sample, timestamp = inlet.pull_sample()
            self.ydata[self.iter_n] = sample[0]
            self.iter_n = (self.iter_n + 1) % self.n_data


    def start_thread(self, *args):
        """Helper function that adds new QRunnables to the threadpool.
        """
        for func in args:
            self.thread = wt.Worker(func)
            self.threadpool.start(self.thread)


    def close_threads(self):
        """Helper function that stops all running threads.
        """
        self._run_thread = False
        self.threadpool.waitForDone()
