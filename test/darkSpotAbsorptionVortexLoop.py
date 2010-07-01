from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Tighter Focus again, f = 11.87: Dark spot absorption vs radius and probe frequency. Trial #1 -- (~366 pm)''')

setvar('imageCropVector',(529,533,150))


setvar('driftTime', 1.5)
setvar('motLoadTime', 250)
setvar('probeIntensity',30)
setvar('voltage1530', 0.87)
setvar('piezoVoltage', 52.6)
setvar('diodeCurrent', 47.5)

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
fastAnalogOut6 = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
camera=dev('Andor iXon 885','ep-timing1.stanford.edu',0)
wavemeter=dev('AndoAQ6140', 'eplittletable.stanford.edu',7)
spectrumAnalyzer=dev('agilentE4411bSpectrumAnalyzer',  'eplittletable.stanford.edu', 18)
probeVortex=dev('Scanning Vortex', 'eplittletable.stanford.edu', 2)


motBlowAway = ch(digitalOut,2)
takeImage=ch(camera,0)
TA2 = ch(fastAnalogOut, 0)
TA3 = ch(fastAnalogOut, 1)
quadCoil = ch(fastAnalogOut, 1)
current1530 = ch(fastAnalogOut6,0)
aomSwitch0 = ch(dds, 0)
wavelength1530=ch(wavemeter, 0)
power1530 = ch(wavemeter, 1)
absoptionLightFrequency = ch(spectrumAnalyzer, 0)
probeVortexCurrent = ch(probeVortex,0)
probeVortexPiezo = ch(probeVortex,1)

# Define different blocks of the experiment
def MOT(Start):

    ## Camera Settings ##
    dtCameraShutter = 0*ms
    dtCameraPulseWidth = 1000*us  + dtCameraShutter
    dtCameraDelay = 5*us
    expTime = 100*us

    ##  Digital Out holdoff  ##
    dtDigitalOutHoldoff = 11*us

    #Initialization Settings
    tStart =dtDigitalOutHoldoff + 2*ms

    #AOM settings
    aomFreq0 = 110
    aomAmplitude0 = probeIntensity #30 / 100 usually
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

    dtAbsorbtionLight = 50*us
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

    ## 1530 Shutter Settings ##
    t1530Off = tTAOff
    dtShutterCloseHoldOff = 2.055*ms
    tShutterOpen = tStart
    tShutterClose = t1530Off - dtShutterCloseHoldOff


    ## End of Sequence Settings ##
    tTAEndOfSequence = tDarkBackground +2*ms

    #################### events #######################
    event(TA2, tStart, 0)    # TA off MOT dark to kill any residual MOT
    event(TA3, tStart, 0)    # TA off
    event(current1530, tStart, voltage1530)    #1530 light on

    event(aomSwitch0,tStart, (aomFreq0, 0 ,0)) # AOM is off, so no imaging light
    event(motBlowAway, tStart, 0)                 #set cooling light to 10 MHz detuned via RF switch

    meas(wavelength1530, tStart)
#    meas(power1530,tStart+500*ms)

    meas(absoptionLightFrequency, tStart)

    ## Load the MOT ##    
    event(TA2, tTAOn, voltageTA2)                   # TA on
    event(TA3, tTAOn, voltageTA3)                   # TA on
    event(TA2, tTAOff, 0)    # TA off
    event(TA3, tTAOff, 0)    # TA off

    ## Take an absorbtion image ##
    event(aomSwitch0, tAomOn, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light
    event(aomSwitch0, tAomOn + dtAbsorbtionLight, (aomFreq0, 0, 0)) #turn off absorbtion light

    meas(takeImage, tAbsorptionCamera, (expTime, description2, filename, imageCropVector))                #take absorption image

    ## Take an abosorbtion calibration image after the MOT has decayed away ##

    event(aomSwitch0, tAomCalibration, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light
    event(aomSwitch0, tAomCalibration + dtAbsorbtionLight, (aomFreq0, 0, 0)) #turn off absorbtion light 

    meas(takeImage, tCalibrationCamera, (expTime,description3,filename, imageCropVector))                #take absorption image

    ## Take a dark background image ##
    meas(takeImage, tDarkBackground, (expTime,description4,filename, imageCropVector))                #take absorption image

    event(TA2, tTAEndOfSequence, voltageTA2)
    event(TA3, tTAEndOfSequence, voltageTA3)

#    event(aomSwitch0, tTAEndOfSequence, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light 

    event(probeVortexCurrent, tDarkBackground, diodeCurrent)
    event(probeVortexPiezo, tTAEndOfSequence, piezoVoltage)

  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
