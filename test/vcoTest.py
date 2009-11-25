from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Take a picture.''')

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
vco0=dev('ADF4360-0', 'ep-timing1.stanford.edu', 0)
vco1=dev('ADF4360-5', 'ep-timing1.stanford.edu', 1)
vco2=dev('ADF4360-5', 'ep-timing1.stanford.edu', 2)
vco3=dev('ADF4360-6', 'ep-timing1.stanford.edu', 3)
trigger=dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)

# Define different blocks of the experiment
def MOT(Start):

    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 20*us, "Pause" )

    event(ch(trigger, 0), 30*us, "Play" )
#    event(ch(trigger, 0), 20*us, "wait for external trigger" )

    event(ch(digitalOut, 0),1*ms, 1 )
    event(ch(digitalOut, 0), 2*ms, 0 )
    event(ch(digitalOut, 0), 3*ms, 1 )
    event(ch(digitalOut, 0), 4*ms, 0 )

    event(ch(digitalOut, 0), 6*ms, 1 )


    event(ch(vco1,0), 4.2*ms, 1350.2)
    event(ch(vco1,0), 5*ms+5.18*ms, 1300.0)
   
    event(ch(vco2,0), 5*ms+400*ms, 1300.0)


#    event(ch(vco1,0), 100*ms, 1248)
#    event(ch(vco1,0), 300*ms, 1300.0)
#    event(ch(vco1,0), 300*ms+80*ms, 1250)
#    event(ch(vco1,0), 300*ms+160*ms, 1251)
#    event(ch(vco1,0), 300*ms+320*ms, 1252)

#    event(ch(vco1,1),400*ms, "-6 dBm")
#    event(ch(vco1,0), 450*ms, 1300.0)
#    event(ch(vco2,1),500*ms, "-6 dBm")
#    event(ch(vco2,0), 550*ms, 1300.0)


#    event(ch(vco2,1), 5*ms, "Off")
#    event(ch(vco3,1), 5*ms+200*ms, "-6 dBm")
#    event(ch(vco3,1), 5*ms+400*ms, "-6 dBm")

#    event(ch(vco2,0), 5*ms+200*ms, 1301.0)

#    event(ch(vco3,0), 5*ms, 1102)

#




#    event(ch(vco2,0), 300*ms+200*us, 1300)

#    event(ch(vco1,0), 3700*ms, 1251) 
#    event(ch(digitalOut,0), 100*ms, 1)
#    event(ch(digitalOut,0), 1*s, 0)

    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
