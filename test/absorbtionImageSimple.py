from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Take a picture.''')

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)

shutter = ch(digitalOut,1)
cameraTrigger=ch(digitalOut,0)
TA2 = ch(fastAnalogOut, 0)
quadCoil = ch(fastAnalogOut, 1)

# Define different blocks of the experiment
def MOT(Start):

    #Initialization Settings
    tStart = 1000*us
    tWait = 1*ms
    
    ## TA Settings ##
    voltageTA = 1.25
    tTAOff =  tStart + tWait

    ## Quad Coil Settings ##
    quadCoilVoltage = 3.01

    ## Camera Settings ##
    dtCameraPulseWidth = 1000*us  
    dtCameraDelay = 5*us

    ## Imaging Settings ##
    dtDriftTime = 0.1*ms
    tImage = tTAOff + dtDriftTime
    tQuadCoilOff = tTAOff
    tCamera = tImage - dtCameraDelay

    ## Calibration Absorbtion Image Settings ##
    dtDeadMOT = 100*ms
    tCalibrationImage = tImage + dtDeadMOT
    tCameraCalibration = tCalibrationImage - dtCameraDelay

    ## Dark background imaging settings ##
    dtWait = 100*ms
    tDarkBackground = tCalibrationImage + dtWait

    ## End of Sequence Settings ##
    tQuadCoilEndOfSequence = tDarkBackground + tWait
    tTAEndOfSequence = tDarkBackground +2*tWait

    #################### events #######################

    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 30*us, "Play" )
    
    event(TA2, tStart, voltageTA)     # TA on
    event(quadCoil, tStart, quadCoilVoltage) #quad coil on
    event(cameraTrigger, tStart, 0)                # initialize Camera Trigger

    event(TA2, tTAOff, 0) #TA off
    event(quadCoil, tQuadCoilOff, 0) #quad coil off

    ## Take an absorbtion image ##

    event(cameraTrigger, tCamera, 1)
    event(cameraTrigger, tCamera + dtCameraPulseWidth, 0)

    ## Take an abosorbtion calibration image after the MOT has decayed away ##

    event(cameraTrigger, tCameraCalibration, 1)
    event(cameraTrigger, tCameraCalibration + dtCameraPulseWidth, 0)

    event(TA2, tTAEndOfSequence, voltageTA)
    event(quadCoil, tQuadCoilEndOfSequence, quadCoilVoltage)

  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
