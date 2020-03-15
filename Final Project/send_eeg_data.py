import time
import numpy as np
from pylsl import StreamInfo, StreamOutlet


"""
Creating a stream with the following properties:
    name = BioSemi
    signal type = EEG
    number of channels = 1 (the first "channel" is the timestamp)
    frequency = 100 Hz
    data type = float32
    unique identifier = eeg_project_123
"""
info = StreamInfo('BioSemi', 'EEG', 1, 100, 'float32', 'eeg_project_123')


# DATA OUTLET
outlet = StreamOutlet(info)


# RETRIEVING DATA
eeg_data = []
with open('eeg_data/data.txt', 'r') as fin:
    for i in fin:
        eeg_data.append(float(i.strip()))
data_length = len(eeg_data)


# SENDING DATA
print("now sending data...")
line_counter = 0
while True:
    # make a new 2-channel sample and send it
    mysample = [eeg_data[line_counter]]
    outlet.push_sample(mysample)

    line_counter = (line_counter + 1) % data_length
    time.sleep(0.01)
