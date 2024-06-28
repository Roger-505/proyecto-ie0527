import math
import matplotlib.pyplot as plt
import numpy as np
import commpy.channels as chan
from commpy.wifi80211 import Wifi80211

# AWGN channel
channels = chan.SISOFlatChannel(None, (1 + 0j, 0j))

w2 = Wifi80211(mcs=1)   # QPSK 1/2
w3 = Wifi80211(mcs=2)   # QPSK 3/4

# SNR range to test
SNRs2 = np.arange(0, 10) + 10 * math.log10(w2.get_modem().num_bits_symbol)
SNRs3 = np.arange(0, 10) + 10 * math.log10(w3.get_modem().num_bits_symbol)

# sim definitions
tx_max = 100
err_min = 10
chunk = 600

# sim
BERs_mcs2 = w2.link_performance(channels, SNRs2, tx_max, err_min, chunk, stop_on_surpass_error=False)
BERs_mcs3 = w3.link_performance(channels, SNRs3, tx_max, err_min, chunk, stop_on_surpass_error=False)

# Extract the first element which is the BER data
BERs_mcs2 = BERs_mcs2[0] 
BERs_mcs3 = BERs_mcs3[0] 

# Test
plt.semilogy(SNRs2, BERs_mcs2, 'o-', SNRs3, BERs_mcs3, 'o-')
plt.grid()
plt.xlabel('Signal to Noise Ratio (dB)')
plt.ylabel('Bit Error Rate')
plt.legend(('MCS 1', 'MCS 2'))
plt.show()
