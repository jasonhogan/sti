from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
#setvar('desc','''Take a picture.''')

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
vco0=dev('ADF4360-0', 'eplittletable.stanford.edu', 0)
vco1=dev('ADF4360-5', 'eplittletable.stanford.edu', 1)
#vco2=dev('ADF4360-5', 'eplittletable.stanford.edu', 2)
vco3=dev('ADF4360-6', 'eplittletable.stanford.edu', 3)
trigger=dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)



blast, cmot, turnOn, mot, none = range(5)


#vcoSetting = mot
vcoSetting=none
#vcoSetting = cmot

# Define different blocks of the experiment
def MOT(Start):



    if(vcoSetting == cmot) :
        event(ch(vco3, 0), 4.2*ms, 1156 )    # use this detuning for a trial CMOT
        event(ch(vco0, 1), 400*ms, "-6 dBm")


    if(vcoSetting == blast) :
        event(ch(vco0, 1), 4.2*ms, "Off" )
        event(ch(vco3, 0), 40.2*ms, 1066+266.65-14 )    # detuned by +14MHz from 2->2'

    if(vcoSetting == mot) :
        event(ch(vco3, 1), 4*ms, "-6 dBm")
        event(ch(vco3, 0), 40.2*ms, 1066 + 10 )    # detuned by -10 MHz 2->3'


    event(ch(vco1, 1), 4*ms, "-6 dBm")
    event(ch(vco1, 0), 40.2*ms, 1332.65)    # depumper: resonant with 2->2'

#    event(ch(vco3, 0), 4.2*ms, 1156 )

#    event(ch(vco3, 0), 4.2*ms, 1122 )
#    event(ch(vco3, 0), 4.2*ms, 1106 )    #Use this for cmot sweep 4/1/2010

#    event(ch(vco3, 0), 4.2*ms, 1066 )


    


#    event(ch(vco3, 1),400*ms, "-6 dBm")

#    event(ch(vco0, 0), 10.2*ms, 2562 )
#    event(ch(vco0, 1),400*ms, "-6 dBm")

#    event(ch(vco0, 1), 400*ms, "-13 dBm")
#    event(ch(vco0, 0), 10.2*ms, 2568 )

#    event(ch(vco2,1), 5*ms, "Off")
#    event(ch(vco3,1), 5*ms+200*ms, "-6 dBm")
#    event(ch(vco3,1), 5*ms+400*ms, "-6 dBm")


    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
