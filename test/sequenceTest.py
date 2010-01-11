from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Take a picture.''')

#fastOut=dev('Fast Analog Out','ep-timing1.stanford.edu',1)
#vco0=dev('ADF4360-0', 'eplittletable.stanford.edu', 0)
#vco1=dev('ADF4360-5', 'eplittletable.stanford.edu', 1)
#vco2=dev('ADF4360-5', 'eplittletable.stanford.edu', 2)
#vco3=dev('ADF4360-6', 'eplittletable.stanford.edu', 3)
trigger=dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds=dev('DDS','ep-timing1.stanford.edu',7)

# Define different blocks of the experiment
def MOT(Start):

    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 30*us, "Play" )

    setvar('a', 2)

#    event(ch(fastOut, 0), 4.2*ms, a )

    event(ch(dds,0),100*ms, [0.1])

    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
