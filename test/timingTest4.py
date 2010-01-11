from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''')

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS_ch0', 'ep-timing1.stanford.edu', 0)

#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now


# Define different blocks of the experiment
def MOT(Start):
    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 20*us, "Pause" )
    event(ch(trigger, 0), 30*us, "Play" )

    
#    event(ch(digitalOut,0), Start+1.5*us, 1)
#    event(ch(digitalOut,0), Start+3*us, 0)
#    event(ch(digitalOut,0), Start+4.5*us, 1)
#

    T = 100*us
#    event(ch(dds,0),T,(1,100,0))
#    event(ch(dds,0),T,(1,50,0))

#    event(ch(dds,0),T,((5,10,1),'',''))
#

    ts = Start + 4.5*us + 100*us

#    event(ch(dds, 0),  ts, (0.1,50,0))


    for i in range(0, 4) :
        event(ch(fastAnalogOut, 0),  ts + T*i+5*us, 0)
        event(ch(fastAnalogOut, 0),  ts + T*i + 0.5*T, 5 )
#        event(ch(digitalOut,0),           ts + T*i, 0)
#        event(ch(digitalOut,0),           ts + T*i+0.5*T, 1)
        event(ch(slowAnalogOut, 0), ts + T*i, 0)
        event(ch(slowAnalogOut, 0), ts + T*i + 0.5*T, 5 )
        event(ch(dds, 0),  ts + T*i-3.625*us, (10,50,0))
#        event(ch(dds, 0),  ts + T*i + 0.5*T, ((0.1,20,0.5*T),'',''))
#        event(ch(dds, 0),  ts + T*i + 0.5*T+5*us, (0.5,0,0))
        event(ch(dds, 0),  ts + T*i + 0.5*T-3.625*us, (0.1,100,0))

#    event(ch(dds, 0),  ts + ts+4*T, ((10,200,1),100,0))

#
#    n=5*2
#    for i in range(1,n) :
#        event(ch(dds, 0),  ts + 2*8*i*T/n, (0.02+0.1*i/n,100,0))


#        event(ch(dds, 0),  ts + 6*T, (0.4,100,0))
#        event(ch(dds, 0),  ts + 10*T, (0.3,100,0))
#        event(ch(dds, 0),  ts + 15*T, (0.4,100,0))
#        event(ch(dds, 0),  ts + 20*T, (0.5,100,0))

#    event(ch(dds, 0),  ts + 5*T, ((0.01,0.02,1*T),100,''))



    event(ch(digitalOut,0),1*s-4*us+5*us, 1)
    event(ch(digitalOut,0), 1*s-2*us+5*us, 0)
    event(ch(digitalOut,0),1*s+5*us, 1)


    deltaDDS = 800*ns
    event(ch(dds, 0),  1*s, (10,100,0))
    event(ch(dds, 0),  1*s+deltaDDS, (100,100,0))
    event(ch(dds, 0),  1*s+2*deltaDDS, (10,100,0))

#    event(ch(dds, 0),  ts + 1*T, ((1,3,1),'',''))
#    event(ch(dds, 0),  ts+13*T, (0.1,100,0))


    fastOffset = 0.2*us
    slowOffset = 10*us


   # event(ch(fastAnalogOut, 0), (t0 + 1*(150*us)), -10 )


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
