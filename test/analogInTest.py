from stipy import *

ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0


time = 100*ms


analogIn = dev('Analog In','ep-timing1.stanford.edu',3)


for i in range(0,10):
    meas(ch(analogIn, 0), time+ 35*us + 5*i*us, 1)


