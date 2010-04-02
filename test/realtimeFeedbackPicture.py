from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Turn off 1530 light immediately before imaging.''')

setvar('1530 freq',1529.367)
setvar('driftTime', 1.5*ms)
setvar('motLoadTime', 250)
setvar('holdoff1530', 3)
setvar('voltage1530', 0.87)
setvar('probeIntensity',30)
#setvar('voltage1530off', 0.87)

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
fastAnalogOut6 = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
vco0=dev('ADF4360-0', 'ep-timing1.stanford.edu', 0)
vco1=dev('ADF4360-5', 'ep-timing1.stanford.edu', 1)
vco2=dev('ADF4360-5', 'ep-timing1.stanford.edu', 2)



shutter = ch(digitalOut,3)
motBlowAway = ch(digitalOut,2)
cameraTrigger=ch(digitalOut,0)
#takeImage=ch(camera,0)

TA2 = ch(fastAnalogOut, 0)
TA3 = ch(fastAnalogOut, 1)
quadCoil = ch(slowAnalogOut, 2)
current1530 = ch(fastAnalogOut6,0)
aomSwitch0 = ch(dds, 0)

#testDevice = ch(slowAnalogOut, 0)

# Define different blocks of the experiment
def MOT(Start):

    ## Camera Settings ##
    dtCameraShutter = 0*ms
    dtCameraPulseWidth = 1000*us  + dtCameraShutter
    dtCameraDelay = 5*us
    expTime = 100*us



    
    ## 1530 Shutter Settings ##
    dtShutterOpenHoldOff = 2.04*ms

    #Initialization Settings
    tStart =Start +dtShutterOpenHoldOff

    ## throwaway image settings ##
    tThrowaway = tStart
    filename1 = 'throwaway image'
    description1 = 'throwaway image'

    #AOM settings
#    absorptionFreq = 1067 
#    aomFreq0 = absorptionFreq / 8
    aomFreq0 = 110
    aomAmplitude0 = probeIntensity #30
    aomHoldOff = 10*us

    ## TA Settings ##
    voltageTA2 = 1.45
    voltageTA3 = 1.5
    tTAOn = tStart + 100*ms
    dtMOTLoad = motLoadTime*ms
    tTAOff =  tTAOn + dtMOTLoad 

    ## Quad Coil Settings ##
    quadCoilVoltage = 3.01

    ## 1530 current settings ##
#    voltage1530 = 0.88

    ## Imaging Settings ##
    dtDriftTime = driftTime
    dtBetweenImages = 275*ms  

    dtAbsorbtionLight = 50*us
    tAbsorptionImage = tTAOff + dtDriftTime
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

    ## 1530 Shutter Settings ##
    t1530Off = tTAOff
    dtShutterCloseHoldOff = 2.055*ms
    tShutterOpen = tStart - dtShutterOpenHoldOff
    tShutterClose = t1530Off - dtShutterCloseHoldOff


    ## End of Sequence Settings ##
#    tQuadCoilEndOfSequence = tDarkBackground + tWait
#    t1530EndOfSequence = tDarkBackground + 2*tWait - dtShutterCloseHoldOff
    tTAEndOfSequence = tAbsorptionImage+expTime+2*ms

    #################### events #######################

#    event(ch(trigger, 1), 10*us, "Stop" )
#    event(ch(trigger, 1), 30*us, "Play" )

#    meas(takeImage, tThrowaway, (expTime,description1),'picture')                #take throwaway image
    event(TA2, tStart, 0)    # TA off MOT dark to kill any residual MOT
    event(TA3, tStart, 0)    # TA off
#    event(current1530, tStart, voltage1530)    #1530 light on

    event(aomSwitch0,tStart, (aomFreq0, 0 ,0)) # AOM is off, so no imaging light
    event(motBlowAway, tStart, 0)                 #set cooling light to 10 MHz detuned via RF switch
#    event(shutter,tStart - dtShutterOpenHoldOff, 1)

    for i in range(0, 25) :

    ## Load the MOT ##    
#        event(ch(digitalOut, 4), tTAOn + i*dtBetweenImages-10*us, 0)
        event(ch(digitalOut, 4), tTAOn + i*dtBetweenImages, 1)
        event(ch(digitalOut, 4), tTAOn + (i+0.5)*dtBetweenImages, 0)

        event(motBlowAway,  tTAOn + i*dtBetweenImages, 0)                 #set cooling light to 10 MHz detuned via RF switch
        event(quadCoil, tTAOn + i*dtBetweenImages, 0.6)
        event(TA2, tTAOn + i*dtBetweenImages, voltageTA2)                   # TA on
        event(TA3, tTAOn + i*dtBetweenImages, voltageTA3)                   # TA on
        event(TA2, tTAOff + i*dtBetweenImages, 0)    # TA off
        event(TA3, tTAOff + i*dtBetweenImages, 0)    # TA off


    ## Take an absorbtion image ##
        event(aomSwitch0, tAomOn + i*dtBetweenImages, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light
        event(aomSwitch0, tAomOn + dtAbsorbtionLight + i*dtBetweenImages, (aomFreq0, 0, 0)) #turn off absorbtion light

    
        event(cameraTrigger, tAbsorptionCamera + i*dtBetweenImages, 1)
        event(cameraTrigger, tAbsorptionCamera+expTime + i*dtBetweenImages, 0)

    ## MOT Blow Away
        
        event(motBlowAway,  tTAEndOfSequence + i*dtBetweenImages, 1) 
        event(TA2, tTAEndOfSequence +i*dtBetweenImages, voltageTA2)
        event(TA3, tTAEndOfSequence +i*dtBetweenImages, voltageTA3)

    ## set  things back to normal

        event(motBlowAway,  tTAEndOfSequence + i*dtBetweenImages + 1*ms, 0) 
        event(TA2, tTAEndOfSequence +i*dtBetweenImages + 1*ms, 0)
        event(TA3, tTAEndOfSequence +i*dtBetweenImages + 1*ms, 0)

        



    event(TA2, tTAEndOfSequence +i*dtBetweenImages + 3*ms, voltageTA2)
    event(TA3, tTAEndOfSequence +i*dtBetweenImages + 3*ms, voltageTA3)


  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
