from stipy import *
 
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
dds = dev('DDS_ch0', 'ep-timing1.stanford.edu', 0)

#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now

shutter = ch(digitalOut,1)
cameraTrigger=ch(digitalOut,0)
TA2 = ch(fastAnalogOut, 0)
quadCoil = ch(fastAnalogOut, 1)
aomSwitch0 = ch(dds, 0)
aomSwitch1 = ch(dds, 1)

# Define different blocks of the experiment
def MOT(Start):

    #Initialization Settings
    tStart = 1000*us
    tWait = 1*ms

    #AOM settings
    aomFreq0 = 83 #83
    aomFreq1 = 77 #77
    aomAmplitude0 = 80
    aomAmplitude1 = 65
    aomHoldOff = 10*us
    
    ## TA Settings ##
    voltageTA = 1.25
    tTAOff =  tStart + tWait

    ## Quad Coil Settings ##
    quadCoilVoltage = 3.01

    ## Camera Settings ##
    dtCameraPulseWidth = 1000*us  
    dtCameraDelay = 5*us
    dtCameraPadding = 0*us

    ## Imaging Settings ##
    dtDriftTime = 5*ms
    dtAbsorbtionLight = 20*us
    dtFluorescenceLight = 180*us
    tImage = tTAOff + dtDriftTime
    tAomOn = tImage - aomHoldOff
    tTAOn = tImage
    tQuadCoilOff = tTAOff
    tCamera = tImage - dtCameraDelay - dtCameraPadding

    ## Calibration Absorbtion Image Settings ##
    dtDeadMOT = 100*ms
    tCalibrationImage = tImage + dtDeadMOT
    tCameraCalibration = tCalibrationImage - dtCameraDelay - dtCameraPadding
    tAomCalibration = tCalibrationImage - aomHoldOff

    ## Dark background imaging settings ##
    dtWait = 100*ms
    tDarkBackground = tCalibrationImage + dtWait

    ## End of Sequence Settings ##
    tQuadCoilEndOfSequence = tDarkBackground + tWait
    tTAEndOfSequence = tDarkBackground + tWait

    #################### events #######################

    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 20*us, "Pause" )
    event(ch(trigger, 0), 30*us, "Play" )

    event(aomSwitch0,tStart, (aomFreq0, aomAmplitude0 ,0)) #first AOM is on
    event(aomSwitch1,tStart + 10*us, (aomFreq1, 0,0)) #second AOM is off, so no imaging light
    
    event(TA2, tStart, voltageTA)     # TA on
    event(quadCoil, tStart, quadCoilVoltage) #quad coil on
    event(cameraTrigger, tStart, 0)                # initialize Camera Trigger

    event(TA2, tTAOff, 0) #TA off
    event(quadCoil, tQuadCoilOff, 0) #quad coil off

    ## Take an absorbtion image ##

    event(cameraTrigger, tCamera, 1)
    event(cameraTrigger, tCamera + dtCameraPulseWidth, 0)

    #event(TA2, tTAOn, voltageTA)    #TA on for flourescence
    #event(TA2, tTAOn + dtFluorescenceLight, 0)        #TA off for flourescence

    event(aomSwitch1, tAomOn, (aomFreq1, aomAmplitude1, 0)) #turn on absorbtion light
    event(aomSwitch1, tAomOn + dtAbsorbtionLight, (aomFreq1, 0, 0)) #turn off absorbtion light 

    ## Take an abosorbtion calibration image after the MOT has decayed away ##

    event(aomSwitch1, tAomCalibration, (aomFreq1, aomAmplitude1, 0)) #turn on absorbtion light
    event(aomSwitch1, tAomCalibration + dtAbsorbtionLight, (aomFreq1, 0, 0)) #turn off absorbtion light 

    event(cameraTrigger, tCameraCalibration, 1)
    event(cameraTrigger, tCameraCalibration + dtCameraPulseWidth, 0)

    ## Take a dark background image ##
    event(cameraTrigger, tDarkBackground, 1)
    event(cameraTrigger,  tDarkBackground + dtCameraPulseWidth, 0)

    event(TA2, tTAEndOfSequence, voltageTA)
    event(quadCoil, tQuadCoilEndOfSequence, quadCoilVoltage)

  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
