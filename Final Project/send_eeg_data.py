"""Example program to demonstrate how to send a multi-channel time series to
LSL."""

import time
import random

from pylsl import StreamInfo, StreamOutlet

# Creating a stream with the following properties:
#   name = BioSemi
#   signal type = EEG
#   number of channels = 1
#   frequency = 100 Hz
#   data type = float32
#   unique identifier = eeg_project_123
info = StreamInfo('BioSemi', 'EEG', 1, 100, 'float32', 'eeg_project_123')

# data outlet
outlet = StreamOutlet(info)

print("now sending data...")
while True:
    # make a new random 1-channel sample and send it
    mysample = [random.random(-64, 64)]
    outlet.push_sample(mysample)
    time.sleep(0.01)
