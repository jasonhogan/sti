from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Turn MOT On.''')

include("channels.py")
include('motFunction.py')

# Define different blocks of the experiment



# Global definitions

t0 = 10*us

time = t0
time =  MOT(t0+1*ms, leaveOn=True, cMOT = False)
