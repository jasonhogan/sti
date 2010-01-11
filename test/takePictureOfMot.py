from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Take a picture.''')

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
#slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
#dds = dev('DDS_ch0', 'ep-timing1.stanford.edu', 0)

#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now

shutter = ch(digitalOut,1)
cameraTrigger=ch(digitalOut,0)

shutterHoldoff=2.05*ms
deltaTImageLightHoldOff = 100*us
deltaTLightOn = 700*us

# Define different blocks of the experiment
def MOT(Start):
    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 20*us, "Pause" )
    event(ch(trigger, 0), 30*us, "Play" )

    tStart = 1000*us+5*ms

    tMotOff = tStart+ 10*ms
    deltaTBallisticExpand = 20*ms
    tImage = tMotOff + deltaTBallisticExpand
    tCameraPulseWidth = 1*ms
    t1530Off = tMotOff-shutterHoldoff



    #event(cameraTrigger, tStart, 0)
    #event(shutter,tStart, 1)    #open shutter
    event(ch(fastAnalogOut, 0),  tStart, 1.1)    #TA on

    #take picture
    #event(cameraTrigger, tMotOff - 1*ms, 1) #trigger camera
    #event(cameraTrigger, tMotOff, 0) #trigger reset to 0

    #MOT and shutter off
    event(ch(fastAnalogOut, 0),  tMotOff, 0)
    #event(shutter,t1530Off, 0)

    #imaging light
    event(ch(fastAnalogOut, 0),  tImage + deltaTImageLightHoldOff, 1.1 )
    event(ch(fastAnalogOut, 0),  tImage + deltaTImageLightHoldOff + deltaTLightOn , 0 )    

    #take picture
    #event(cameraTrigger, tImage, 1)

    #cleanup
    #event(cameraTrigger, tImage + tCameraPulseWidth, 0)
    event(ch(fastAnalogOut, 0),  tImage + 2*tCameraPulseWidth , 1.1 )
    #event(shutter, tImage + 2*tCameraPulseWidth , 1)    #open shutter

  
    return Start


# Global definitions


t0 = 10*us

time = t0
time = MOT(time)
