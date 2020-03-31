# ===================================
#   Name: Edward (Eddie) Guo
#   ID: 1576381
#   Partner: Jason Kim
#   CMPUT 275, Fall 2020
#
#   Final Assignment: EEG Visualizer
# ===================================

"""
Simple routine that sends EEG data from a text file over a local network.
The data was retrieved from https://raphaelvallat.com/bandpower.html. This
subject was recorded with a 100 Hz EEG for the F3 electrode (left side of head
towards the rostral region of the frontal cortex). Here, the subject was
recorded in the N3 phase of sleep for 30 s. The program continually loops the
data via Lab Streaming Layer (LSL).
"""

import time
import numpy as np
from pylsl import StreamInfo, StreamOutlet


# Creating a data stream with the following properties:
#     name = BioSemi
#     signal type = EEG
#     number of channels = 1
#     frequency = 100 Hz
#     data type = float32
#     unique identifier = eeg_project_123
info = StreamInfo('BioSemi', 'EEG', 1, 100, 'float32', 'eeg_project_123')


# DATA OUTLET
outlet = StreamOutlet(info)


# RETRIEVING DATA FROM TEXT FILE
eeg_data = []
with open('eeg_data/data.txt', 'r') as fin:
    for line in fin:
        eeg_data.append(float(line.strip()))
data_length = len(eeg_data)


# SENDING DATA
print("Now sending data...")
line_counter = 0
while True:
    # make a new 1-channel sample and send it
    mysample = [eeg_data[line_counter]]
    outlet.push_sample(mysample)
    # create a continual data loop to simulate a "live" EEG recording
    line_counter = (line_counter + 1) % data_length
    time.sleep(0.01)
