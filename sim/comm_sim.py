import numpy as np
import commpy 
from commpy.wifi80211 import Wifi80211
from commpy.channels import _FlatChannel

wifi = Wifi80211(2)

# Define channel to be used in the simulation
channel = _FlatChannel

# Define the SNRs (in dB) to simulate
SNRs = np.arange(0, 21, 2)  # SNR from 0 to 20 dB in steps of 2 dB

# Define the maximum number of transmissions and minimum number of errors
tx_max = 20000  # Maximum number of transmissions per SNR
err_min = 100   # Minimum number of errors to accumulate per SNR

# Run the simulation
BERs = wifi.link_performance(channel, SNRs, tx_max, err_min)
