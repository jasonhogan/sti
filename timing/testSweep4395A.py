from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

#include('channels.py')

spectrumAnalyzer4395A = dev("Network Analyzer 4395A", "eplittletable.stanford.edu", 10)
knifeFreq = ch(spectrumAnalyzer4395A, 0)

setvar('desc', "Test sweep")


tStart = 100*ms

#event(knifeFreq, tStart, ((360, 400, 2.5), (460, 500, 1)))
#event(knifeFreq, tStart, (460, 489, 3))

event(knifeFreq, tStart, (380, 500, 10))
