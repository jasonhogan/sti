from stipy import *

ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0


time = 100*ms


digital = dev('Digital Out','ep-timing2.stanford.edu',2)


event(ch(digital, 2), time+100*ms, 1 )




