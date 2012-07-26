from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''') 

#trigger = dev('FPGA_Trigger', 'timing-test.stanford.edu', 8)
#dds = dev('DDS', 'timing-test.stanford.edu', 7)


#digitalOut = dev('Digital Out', 'timing-test.stanford.edu',2)
#fastAnalogOut=dev('Fast Analog Out', 'timing-test.stanford.edu', 6)

trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
#digitalOut = dev('Digital Out', 'ep-timing1.stanford.edu',2)
#fastAnalogOut=dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)

setvar('ddsFreq',1)


# Define different blocks of the experiment
def MOT(Start):



#    event(ch(trigger,8), 5*ns, "Wait")

#    Fstart=180+0.0063*6

#    event(ch(dds, 2), Start, 1 )
#    event(ch(dds, 2), Start+20*ms, ((180,120,10*ms), 100, 0) )

#    event(ch(dds, 2), Start+100*ms, (125, 100, 0) )

#    event(ch(dds, 2), Start+200*ms, (((80,100,10*ms),(100,110,2*ms),(110,120,20*ms),(120,145,10*ms)), 40, 0) )
#    event(ch(dds, 2), Start+500*ms, ((125,10,10*s), 100, 0) )
#    event(ch(dds, 2), Start+100*ms, (((125,100,1*s),(100,10,10*s)), 100, 0) )

#    event(ch(dds, 2), Start+10*ms, (170, 100, 0) )
#    event(ch(dds, 2), Start+30*ms, (160, 100, 0) )
#    event(ch(dds, 2), Start+50*ms, (150, 100, 0) )
#
#    event(ch(dds, 2), Start+12*s, (125, 100, 0) )
        
#    event(ch(fastAnalogOut, 1), Start+20*ms, 2.2)

#    event(ch(dds, 0), Start, ((Fstart, Fstart + 0.09, 1*s), 100, 0) )


#    event(ch(dds, 0), Start, ((Fstart, Fstart + 20, 1*s), 100, 0) )



#    event(ch(dds, 1), 1*ms, ( 10.3, 100, 0 ) )
#    event(ch(dds, 1), 1*ms, ((200,150,1*s), 100, 50) )


#    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 20*us, "Play" )

#    tDDS=Start+55*us

#    event(ch(digitalOut, 4), Start+50*us, 0)
#    event(ch(digitalOut, 4), Start+ 55*us, 1)
#    event(ch(digitalOut, 4), Start + 60*us, 0)

#    event(ch(dds, 0), Start + 55*us, ((1,10,100*us), 100, 0 ) )
#    event(ch(dds, 0), Start + 55*us, (ddsFreq, 100, 0 ) )
#    event(ch(fastAnalogOut, 0), Start + 55*us, 0 )
#    event(ch(fastAnalogOut, 0), Start + 70*us, 5 )
#    event(ch(dds, 0), Start + 55*us, (1, 100, 0 ) )


#    VCOScaleFactor=112.19421

#    VCOScaleFactor=112.19421

    VCOScaleFactor=110.59429

#    event(ch(dds, 1), tDDS, ( 10.3, 100, 0 ) )
#    event(ch(dds, 1), tDDS, ( (1076+80)/VCOScaleFactor, 100, 0 ) )
#    event(ch(dds, 1), tDDS, ( 1076/VCOScaleFactor, 100, 0 ) )

 #    event(ch(dds, 1), tDDS, ( (9.59,10.3,1*s), 100, 0 ) )
#    event(ch(dds, 1), tDDS+100*us, ( 11, 0, 0 ) )

#    setvar('fMOTCapture',1076)
#
#    event(ch(dds, 1), tDDS, ( (1076/VCOScaleFactor, (1076+80)/VCOScaleFactor, 10*ms), 100, 0 ) )
#    event(ch(dds, 1), tDDS, ( 1076/VCOScaleFactor, 100, 0 ) )

#    event(ch(dds, 1), tDDS, ( (1076/VCOScaleFactor, (1076+80)/VCOScaleFactor, 1*s), 100, 0 ) )


#    event(ch(dds, 2), 100*ms, ( 75, 90, 0 ) )
#    event(ch(dds, 2), 100*ms, ( 75, 0, 0 ) )

#    event(ch(dds, 0), 10*s + 80*us, ( (150,25,5*s), 100, 0 ) )
#    event(ch(dds, 0), 4010*ms, ( (1,100,1000*ms), 100, 0 ) )
#    event(ch(dds, 0), 3*ms, (110, 30, 0) )
#    event(ch(dds, 0), 10*ms + 40*us, (.035,100,0))
#    event(ch(digitalOut, 0), 7010*ms, 1)
#    event(ch(digitalOut, 0), 7010*ms + 40*us, 0)

    time = Start+50*ms
    
#    event(ch(dds, 2), time, ((90, 10, 0.25*s), 100, 0) )

#    event(ch(dds, 3), time+0.3*s, (((120,40,5*s),(40,90,5*s)), 100, 0) )    #doesn't work; goes between 120->90 directly
    
#    event(ch(dds, 3), time+0.3*s, (((40,120,5*s),(120,90,5*s)), 100, 0) )    #doesn't work




##Sweep up then down
#    event(ch(dds, 3), time, ((40,120,5*s), 100, 0) )
#    time += 5*s + 9*ms
#    event(ch(dds, 3), time, (120, 100, 0) )
#    time += 11*us + 11*us
##    time += 11*us    #doesn't work: swepps back to 40 and doesn't stop
#    event(ch(dds, 3), time, ((120,90,2*s), 100, 0) )


#
##Sweep down then up
#    event(ch(dds, 3), time, ((120,40, 5*ms), 100, 0) )
#    time += 5*ms + 10*us
#    event(ch(dds, 3), time, (40, 100, 0) )
#    time += 11*us  
#    event(ch(dds, 3), time, ((40,90,2*ms), 100, 0) )

 
    #event(ch(dds, 2), time, (((120,40,1*s),(40,40,10*us),(40,90,1*s)), 100, 0) )

#    event(ch(dds, 2), time, (((40,120,1*s),(120,120,10*us),(120,90,1*s)), 100, 0) )

#    #This works!!
#    event(ch(dds, 2), time, ((10, 90, 5*s), 100, 0) )
#    event(ch(dds, 3), time+20*us, ((40, 60, 3*s), 100, 0) )

    event(ch(dds, 2), time+20*us, (((10, 30, 1*s),(30, 31, 2*s),(30,60,1*s)), 100, 0) )
    event(ch(dds, 3), time, ((120, 5, 3*s), 100, 0) )
    
    return Start


# Global definitions

 
t0 = 10*us

time = t0
time = MOT(time+100*us)
