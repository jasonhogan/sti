from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Take a picture.''')

#slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
camera=dev('Andor iXon 885','ep-timing1.stanford.edu',0)

#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now

TA2 = ch(fastAnalogOut, 0)
TA3 = ch(fastAnalogOut, 1)
aomSwitch0 = ch(dds, 0)
takeImage=ch(camera,0)

shutterHoldoff=2.05*ms
deltaTImageLightHoldOff = 100*us
deltaTLightOn = 700*us

# Define different blocks of the experiment
def MOT(Start):
    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 30*us, "Play" )

    tStart = 1.1*s

    expTime = 10*ms

    tImage = tStart+ 10*ms
#    tTrigger = tStart + 5*us
#    tTrigger1 = tTrigger + dtWidth
#    tTrigger2 = tTrigger1 + dtWidth
#    tTrigger3 = tTrigger2 + dtWidth
#    tTAInitOff = tTrigger
    tCleanup = 1000*ms

    voltageTA = 1

    #AOM settings
#    absorptionFreq = 1067 
#    aomFreq0 = absorptionFreq / 8
    aomFreq0 = 110
    aomAmplitude0 = 100
    aomHoldOff = 10*us

    tTAOff = tStart+5*ms

##events##

    
#    event(ch(fastAnalogOut, 0), tTAInitOff, 0)     # TA off
#    event(cameraTrigger, tTAInitOff, 0)                # initialize Camera Trigger
    
#    # Wiggle trigger/shutter so the oscilloscope triggers
#    event(shutter, tTrigger, 1)
#    event(shutter, tTrigger1, 0)
#    event(shutter, tTrigger2, 1)
#    event(shutter, tTrigger3, 0)

    
#    tImage = tTrigger3 + 1*ms
#    dtCameraPulseWidth = 1*ms  

    event(TA2, tTAOff, 0)    # TA off
    event(TA3, tTAOff, 0)    # TA off

    dtIncrement = 250*us
    dtInitLaserOffset = - 3*dtIncrement
    dtAbsorbtionLight = 1*ms
    
    dtDelay = 75*ms


    for i in range(0,10):
        tAOMOn = tImage + dtInitLaserOffset + i*dtIncrement
        event(takeImage, tImage, (expTime,'calibration'))

        event(aomSwitch0, tAOMOn, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light
        event(aomSwitch0, tAOMOn + dtAbsorbtionLight, (aomFreq0, 0, 0)) #turn off absorbtion light 

        tImage += dtDelay
        
              

    # Take a picture
    
   
    

    #Cleanup

  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)

