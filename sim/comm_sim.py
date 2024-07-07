import math
import matplotlib.pyplot as plt
import numpy as np
import commpy.channels as chan
from commpy.wifi80211 import Wifi80211

# AWGN channel
channels = chan.SISOFlatChannel(None, (1 + 0j, 0j))

w0 = Wifi80211(mcs=0)   # BPSK
w1 = Wifi80211(mcs=1)   # QPSK 1/2
w2 = Wifi80211(mcs=2)   # QPSK 3/4
w3 = Wifi80211(mcs=3)   # 16-QAM 1/2
w4 = Wifi80211(mcs=4)   # 16-QAM 3/4
w5 = Wifi80211(mcs=5)   # 64-QAM 2/3
w6 = Wifi80211(mcs=6)   # 64-QAM 3/4
w7 = Wifi80211(mcs=7)   # 64-QAM 5/6

# SNR range to test
SNRs0 = np.linspace(0, 10, 100) + 10 * math.log10(w0.get_modem().num_bits_symbol)
SNRs1 = np.linspace(0, 10, 100) + 10 * math.log10(w1.get_modem().num_bits_symbol)
SNRs2 = np.linspace(0, 10, 100) + 10 * math.log10(w2.get_modem().num_bits_symbol)
SNRs3 = np.linspace(0, 10, 100) + 10 * math.log10(w3.get_modem().num_bits_symbol)
SNRs4 = np.linspace(0, 10, 100) + 10 * math.log10(w4.get_modem().num_bits_symbol)
SNRs5 = np.linspace(0, 10, 100) + 10 * math.log10(w5.get_modem().num_bits_symbol)
SNRs6 = np.linspace(0, 10, 100) + 10 * math.log10(w6.get_modem().num_bits_symbol)
SNRs7 = np.linspace(0, 10, 100) + 10 * math.log10(w7.get_modem().num_bits_symbol)

# sim definitions
tx_max = 2000    # 20000
err_min = 10    # 10
chunk = 8       # 600

# sim
BERs_mcs0 = w1.link_performance(channels, SNRs0, tx_max, err_min, chunk, stop_on_surpass_error=False)
BERs_mcs1 = w1.link_performance(channels, SNRs1, tx_max, err_min, chunk, stop_on_surpass_error=False)
BERs_mcs2 = w1.link_performance(channels, SNRs2, tx_max, err_min, chunk, stop_on_surpass_error=False)
BERs_mcs3 = w1.link_performance(channels, SNRs3, tx_max, err_min, chunk, stop_on_surpass_error=False)
BERs_mcs4 = w1.link_performance(channels, SNRs4, tx_max, err_min, chunk, stop_on_surpass_error=False)
BERs_mcs5 = w1.link_performance(channels, SNRs5, tx_max, err_min, chunk, stop_on_surpass_error=False)
BERs_mcs6 = w1.link_performance(channels, SNRs6, tx_max, err_min, chunk, stop_on_surpass_error=False)
BERs_mcs7 = w1.link_performance(channels, SNRs7, tx_max, err_min, chunk, stop_on_surpass_error=False)

# Extract the first element which is the BER data
BERs_mcs0 = BERs_mcs0[0] 
BERs_mcs1 = BERs_mcs1[0] 
BERs_mcs2 = BERs_mcs2[0] 
BERs_mcs3 = BERs_mcs3[0] 
BERs_mcs4 = BERs_mcs4[0] 
BERs_mcs5 = BERs_mcs5[0] 
BERs_mcs6 = BERs_mcs6[0] 
BERs_mcs7 = BERs_mcs7[0] 

# Test
plt.semilogy (SNRs0, BERs_mcs0, 'o-', SNRs1, BERs_mcs1, 'o-', SNRs2, BERs_mcs2, 'o-', SNRs3, BERs_mcs3, 'o-', SNRs4, BERs_mcs4, 'o-', SNRs5, BERs_mcs5, 'o-', SNRs6, BERs_mcs6, 'o-', SNRs7, BERs_mcs7, 'o-')
plt.grid()
plt.xlabel('SNR (dB)')
plt.ylabel('BER')
plt.legend(('BPSK', 'QPSK 1/2', '16-QAM 1/2', '16-QAM 3/4', '64-QAM 2/3', '64-QAM 3/4', '64-QAM 5/6'))
plt.show()
