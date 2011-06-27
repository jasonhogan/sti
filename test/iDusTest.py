from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test iDus camera.''')

include("channels.py")

# Define different blocks of the experiment
def iDusTest(Start):
    iDusCamera = dev('Andor iDus','ep-timing1.stanford.edu',0)
    camera = ch(iDusCamera, 0)
    print time
    time = takeFluorescenceImage(time, time + dtDeadMOT, cropVector=(256,256,250))

    return Start


# Global definitions

t0 = 10*us

time = t0
time = iDusTest(time)