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

    tTrigger = tStart+ 10*ms
    dtWidth = 1*ms
    tTrigger1 = tTrigger + dtWidth
    tTrigger2 = tTrigger1 + dtWidth
    tTrigger3 = tTrigger2 + dtWidth
    tTAInitOff = tTrigger
    tCleanup = 1000*ms

    voltageTA = 1
    
    event(ch(fastAnalogOut, 0), tTAInitOff, 0)     # TA off
    event(cameraTrigger, tTAInitOff, 0)                # initialize Camera Trigger
    
    # Wiggle trigger/shutter so the oscilloscope triggers
    event(shutter, tTrigger, 1)
    event(shutter, tTrigger1, 0)
    event(shutter, tTrigger2, 1)
    event(shutter, tTrigger3, 0)

    
    tImage = tTrigger3 + 1*ms
    dtCameraPulseWidth = 1*ms  

    dtExposureTime = 5*us
    dtIncrement = 2*us
    dtInitLaserOffset = dtExposureTime - 5*dtIncrement
    dtTAOnWidth = 10*us
    
    dtDelay = 50*ms


    for i in range(0,10):
        tTAOn = tImage + dtInitLaserOffset - i*dtIncrement
        event(cameraTrigger, tImage, 1)
        event(cameraTrigger, tImage + dtCameraPulseWidth, 0)

        event(ch(fastAnalogOut, 0), tTAOn, voltageTA)    #TA on
        event(ch(fastAnalogOut,0), tTAOn + dtTAOnWidth, 0)        #TA off

        tImage += dtDelay
        
              

    # Take a picture
    
   
    

    #Cleanup
    event(cameraTrigger, tCleanup, 0)
    event(shutter, tCleanup, 0)
  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)

