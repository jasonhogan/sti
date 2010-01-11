from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''')

mux = dev('Agilent34970a', 'Agilent34970a@li-gpib.stanford.edu', 15)


t0 = 0*s

event(ch(mux,0), t0,0)


# Global definitions
