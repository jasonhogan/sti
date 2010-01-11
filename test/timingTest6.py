from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''')

digitalOut=dev('Digital Out','timing-test.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'timing-test.stanford.edu', 4)
#fastAnalogOut6 = dev('Fast Analog Out', 'timing-test.stanford.edu', 6)
fastAnalogOut1 = dev('Fast Analog Out', 'timing-test.stanford.edu', 1)
trigger = dev('FPGA_Trigger', 'timing-test.stanford.edu', 8)
dds = dev('DDS_ch0', 'timing-test.stanford.edu', 0)

#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now


# Define different blocks of the experiment
def MOT(Start):
    event(ch(trigger, 0), 10*us, "Stop" )
 #   event(ch(trigger, 0), 20*us, "Play" )
    event(ch(trigger, 0), 20*us, "wait for external trigger" )

    event(ch(fastAnalogOut1,0), Start + 1.5*us, 4.5)
#    event(ch(fastAnalogOut1,1), Start + 1.5*us, 5)
#    event(ch(fastAnalogOut6,0), Start + 1.5*us, 0)
#    event(ch(fastAnalogOut6,1), Start + 1.5*us, 0)

    
#    event(ch(digitalOut,0), Start+1.5*us, 1)
#    event(ch(digitalOut,0), Start+1.5*us+1*s, 0)
#    event(ch(digitalOut,0), Start+1.5*us+2*s, 1)
#    event(ch(digitalOut,0), Start+1.5*us+3*s, 0)
#    event(ch(digitalOut,0), Start+1.5*us+4*s, 1)
#    event(ch(digitalOut,0), Start+1.5*us+5*s, 0)
#    event(ch(digitalOut,0), Start+1.5*us+6*s, 1)


#    for i in range(0, 4) :
#        event(ch(fastAnalogOut, 0),  ts + T*i+5*us, 0)
#        event(ch(fastAnalogOut, 0),  ts + T*i + 0.5*T, 5 )
#        event(ch(digitalOut,0),           ts + T*i, 0)
#        event(ch(digitalOut,0),           ts + T*i+0.5*T, 1)
#        event(ch(slowAnalogOut, 0), ts + T*i, 0)
#        event(ch(slowAnalogOut, 0), ts + T*i + 0.5*T, 5 )
#        event(ch(dds, 0),  ts + T*i-3.625*us, (10,50,0))
#        event(ch(dds, 0),  ts + T*i + 0.5*T, ((0.1,20,0.5*T),'',''))
#        event(ch(dds, 0),  ts + T*i + 0.5*T+5*us, (0.5,0,0))
#        event(ch(dds, 0),  ts + T*i + 0.5*T-3.625*us, (0.1,100,0))



#    for i in range(0, 10) :
#        event(ch(slowAnalogOut, 0), (t0 + i*(150*ms) + fastOffset), -10 )
#        event(ch(slowAnalogOut, 0), (t0 + i*(150*ms)+50.5*ms + fastOffset), 10 )

#    for i in range(0, 200) :
#        event(ch(slowAnalogOut, 0), (2*s+t0 + i*(150*ms) + fastOffset), -10 )
#        event(ch(slowAnalogOut, 0), (2*s+t0 + i*(150*ms)+1.5*ms + fastOffset), 10 )

#    event(ch(trigger, 0),1*s, "Stop" )
#    event(ch(trigger, 0),2*s, "Stop" )
#    event(ch(trigger, 0),3*s, "Stop" )
#    event(ch(trigger, 0),4*s, "Stop" )
#    event(ch(trigger, 0),5*s, "Stop" )

    return Start


# Global definitions


t0 = 10*us

time = t0
time = MOT(time)
