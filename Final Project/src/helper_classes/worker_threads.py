# ===================================
#   Name: Edward (Eddie) Guo
#   ID: 1576381
#   Partner: Jason Kim
#   CMPUT 275, Fall 2020
#
#   Final Assignment: EEG Visualizer
# ===================================

"""
Contains helper classes for multi-thread maintenance. Signals from the
QRunnable instances are captured by WorkerSignals.
"""

from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import traceback, sys


class WorkerSignals(QObject):

    '''Defines the signals available from a running worker thread.

    Attributes:
        finished: No data
        error (tuple): (exctype, value, traceback.format_exc())
        result (object): data returned from processing, anything
    '''
    finished = pyqtSignal()
    error = pyqtSignal(tuple)
    result = pyqtSignal(object)


class Worker(QRunnable):

    '''Worker thread that inherits from QRunnable to handle worker thread
    setup, signals and wrap-up.
    '''
    def __init__(self, fn, *args, **kwargs):
        super(Worker, self).__init__()

        # Store constructor arguments (re-used for processing)
        self.fn = fn
        self.args = args
        self.kwargs = kwargs
        self.signals = WorkerSignals()    


    @pyqtSlot()
    def run(self):
        '''Initialize the runner function with passed args, kwargs. Starts the
        thread for the QRunnable.
        '''
        try:
            result = self.fn(*self.args, **self.kwargs)
        except:
            traceback.print_exc()
            exctype, value = sys.exc_info()[:2]
            self.signals.error.emit((exctype, value, traceback.format_exc()))
        else:
            self.signals.result.emit(result)
        finally:
            self.signals.finished.emit()


    @pyqtSlot()
    def stop(self):
        """Terminates all thread processes.
        """
        self.terminate()
