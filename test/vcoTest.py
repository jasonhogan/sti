from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Take a picture.''')

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
vco0=dev('ADF4360-0', 'eplittletable.stanford.edu', 0)
#vco1=dev('ADF4360-5', 'eplittletable.stanford.edu', 1)
#vco2=dev('ADF4360-5', 'eplittletable.stanford.edu', 2)
vco3=dev('ADF4360-6', 'eplittletable.stanford.edu', 3)
trigger=dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)

# Define different blocks of the experiment
def MOT(Start):

    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 30*us, "Play" )


#    event(ch(vco3, 0), 4.2*ms, 1074.5 )
    #event(ch(vco3, 1),400*ms, "-11 dBm")

#    event(ch(vco0, 0), 10.2*ms, 2562 )
    event(ch(vco0, 1),400*ms, "-6 dBm")


#    event(ch(vco2,1), 5*ms, "Off")
#    event(ch(vco3,1), 5*ms+200*ms, "-6 dBm")
#    event(ch(vco3,1), 5*ms+400*ms, "-6 dBm")


    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
