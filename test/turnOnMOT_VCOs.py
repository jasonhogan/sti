from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Take a picture.''')

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
vco0=dev('ADF4360-0', 'ep-timing1.stanford.edu', 0)
#vco1=dev('ADF4360-5', 'ep-timing1.stanford.edu', 1)
#vco2=dev('ADF4360-5', 'ep-timing1.stanford.edu', 2)
trigger=dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)

# Define different blocks of the experiment
def MOT(Start):

    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 30*us, "Play" )

    #initialize Repump VCO (ADF4360-0)
    event(ch(vco0,1), 5*ms, "-6 dBm")
#    event(ch(vco0,0), 25*ms, 2555)

    #initialize 87 Cooling VCO (ADF4360-5)
#    event(ch(vco1,1), 15*ms, "-6 dBm")
#    event(ch(vco1,0), 30*ms, 1464.3)


    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
