from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''')

#digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
#fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
analogIn = dev('Analog In', 'ep-timing1.stanford.edu', 3)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
#dds = dev('DDS_ch0', 'ep-timing1.stanford.edu', 0)
#
#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now


# Define different blocks of the experiment
def MOT(Start):

    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 30*us, "Play" )
#    event(ch(trigger, 0), 50*us, "Wait" )

#    event(ch(digitalOut,0), Start+1.5*ms, 1)
#    event(ch(digitalOut,0), Start+3*ms, 0)
#    event(ch(digitalOut,0), Start+4.5*ms, 1)
#    event(ch(digitalOut,0), Start+7.5*ms, 0)

    ts = Start + 4.5*us + 100*us
    T = ts+100*ms
#    event(ch(fastAnalogOut, 0),  ts + T, 3.5)

    event(ch(slowAnalogOut, 1), ts, 0.5)
    meas(ch(analogIn,0),ts)

    delta = 10*ms
    for i in range(0, 5) :
        event(ch(slowAnalogOut, 1), T+delta*i, i)
        meas(ch(analogIn,0),T+delta*i+1000*us)

    meas(ch(analogIn,0),20*s)

#    for i in range(0, 4) :
#        event(ch(fastAnalogOut, 0),  ts + T*i, 0)
#        event(ch(fastAnalogOut, 0),  ts + T*i + 0.5*T, 5 )
#        event(ch(slowAnalogOut, 0), ts + T*i, 0)
#        event(ch(slowAnalogOut, 0), ts + T*i + 0.5*T, 5 )

#    event(ch(trigger, 0), 1*s, "Stop" )


    return Start


# Global definitions


t0 = 10*us

time = t0
time = MOT(time)
