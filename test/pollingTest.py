from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''') 

trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
#dds = dev('DDS', 'ep-timing1.stanford.edu', 7)
digitalOut = dev('Digital Out', 'ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)



# Define different blocks of the experiment
def MOT(Start):

    event(ch(digitalOut, 0), Start, 0 )
    event(ch(digitalOut, 0), Start+1*ms, 1 )
    event(ch(digitalOut, 0), Start+2*ms, 1 )
    event(ch(digitalOut, 0), Start+10*ms, 0 )
    event(ch(digitalOut, 0), Start+100*ms, 1 )
    
    event(ch(slowAnalogOut, 25), Start+10*ms, 0)
    event(ch(slowAnalogOut, 25), Start+100*ms, 0.1)

    for i in range(1,40) :
        event(ch(slowAnalogOut, 25), Start+100*ms+0.2*ms*i, 0)

#    event(ch(slowAnalogOut, 25), Start+5002*ms, 0)


#    event(ch(slowAnalogOut, 25), 4*s, 0)
#    event(ch(slowAnalogOut, 25), 6.01*s, 0)

#    event(ch(digitalOut, 0), Start+10000*ms, 0 )


    return Start


# Global definitions

 
t0 = 10*us

time = t0
time = MOT(time+100*us)
