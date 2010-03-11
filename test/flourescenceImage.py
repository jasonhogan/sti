from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Take a calibrated fluorescence''')

setvar('driftTime', 1)
setvar('motLoadTime', 100)

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
fastAnalogOut6 = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
vco0=dev('ADF4360-0', 'ep-timing1.stanford.edu', 0)
vco1=dev('ADF4360-5', 'ep-timing1.stanford.edu', 1)
vco2=dev('ADF4360-5', 'ep-timing1.stanford.edu', 2)
camera=dev('Andor iXon 885','ep-timing1.stanford.edu',0)


shutter = ch(digitalOut,3)
motBlowAway = ch(digitalOut,2)
takeImage=ch(camera,0)

TA2 = ch(fastAnalogOut, 0)
TA3 = ch(fastAnalogOut, 1)
quadCoil = ch(fastAnalogOut, 1)
aomSwitch0 = ch(dds, 0)
#repumpVCO=dev('ADF4360-0', 'eplittletable.stanford.edu', 0)
#coolingVCO=dev('ADF4360-6', 'eplittletable.stanford.edu', 3)

# Define different blocks of the experiment
def MOT(Start):

    ## Camera Settings ##
    dtCameraShutter = 0*ms
    dtCameraPulseWidth = 1000*us  + dtCameraShutter
    dtCameraDelay = 5*us
    expTime = 500*us



    
    ## Shutter Settings ##
    dtShutterOpenHoldOff = 2.04*ms

    #Initialization Settings
    tStart =1.1*s +dtShutterOpenHoldOff

    ## throwaway image settings ##
    tThrowaway = tStart
    filename1 = 'throwaway image'
    description1 = 'throwaway image'

    #AOM settings
    aomFreq0 = 110
    aomAmplitude0 = 30
    aomHoldOff = 10*us

    ## TA Settings ##
    voltageTA2 = 1.4
    voltageTA3 = 1.5
    tTAOn = tStart + 100*ms
    dtMOTLoad = motLoadTime*ms
    tTAOff =  tTAOn + dtMOTLoad 

    ## Quad Coil Settings ##
    quadCoilVoltage = 3.01

    ## Imaging Settings ##
    dtDriftTime = driftTime*ms   

    dtAbsorbtionLight =expTime
    tAbsorptionImage = tTAOff + dtDriftTime - dtCameraShutter
    tAomOn = tTAOff + dtDriftTime - aomHoldOff
    tQuadCoilOff = tTAOff
    tAbsorptionCamera = tAbsorptionImage - dtCameraDelay
    filename = 'absorption image'
    description2 = 'absorption image'

    ## Calibration Absorbtion Image Settings ##
    dtDeadMOT = 100*ms
    tCalibrationImage = tAbsorptionImage + dtDeadMOT
    tCalibrationCamera = tCalibrationImage - dtCameraDelay
    tAomCalibration = tCalibrationImage - aomHoldOff
    description3 = 'calibration image'

    ## Dark background imaging settings ##
    dtWait = 100*ms
    tDarkBackground = tCalibrationImage + dtWait
    description4 = 'background image'


    ## End of Sequence Settings ##
#    tQuadCoilEndOfSequence = tDarkBackground + tWait
    tTAEndOfSequence = tDarkBackground +2*ms

    #################### events #######################

    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 30*us, "Play" )

    meas(takeImage, tThrowaway, (expTime,description1),'picture')                #take throwaway image
    event(TA2, tStart, 0)    # TA off MOT dark to kill any residual MOT
    event(TA3, tStart, 0)    # TA off

    event(aomSwitch0,tStart, (aomFreq0, 0 ,0)) # AOM is off, so no imaging light
    event(motBlowAway, tStart, 0)                 #set cooling light to 10 MHz detuned via RF switch
    #event(shutter,tStart - dtShutterOpenHoldOff, 1)


    ## Load the MOT ##    
    event(TA2, tTAOn, voltageTA2)                   # TA on
    event(TA3, tTAOn, voltageTA3)                   # TA on
    event(TA2, tTAOff, 0)    # TA off
    event(TA3, tTAOff, 0)    # TA off


    ## Take a fluorescence image ##
    event(TA2, tAomOn,voltageTA2) #turn on fluorescence light
    event(TA2, tAomOn + dtAbsorbtionLight,0) #turn off fluorescence light

    meas(takeImage, tAbsorptionCamera, (expTime, description2, filename))                #take fluorescence image

    ## Take a fluorescence calibration image after the MOT has decayed away ##

    event(TA2,  tAomCalibration,voltageTA2) #turn on fluorescence light
    event(TA2, tAomCalibration + dtAbsorbtionLight,0) #turn off fluorescence light 

    meas(takeImage, tCalibrationCamera, (expTime,description3,filename))                #take absorption image

    ## Take a dark background image ##
    meas(takeImage, tDarkBackground, (expTime,description4,filename))                #take absorption image

    event(TA2, tTAEndOfSequence + 1*s, voltageTA2)
    event(TA3, tTAEndOfSequence  + 1*s, voltageTA3)
  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
