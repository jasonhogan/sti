from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Exploring dark spot rings, now all spectrum analyzer freqs are negative.''')

#setvar('1530 freq',1529.367)
setvar('driftTime', 1.5)
setvar('motLoadTime', 250)
setvar('probeIntensity',30)
#setvar('holdoff1530', 3)
setvar('voltage1530', 0.87)
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
camera=dev('Andor iXon 885','ep-timing1.stanford.edu',0)
wavemeter=dev('AndoAQ6140', 'eplittletable.stanford.edu',7)
spectrumAnalyzer=dev('agilentE4411bSpectrumAnalyzer',  'eplittletable.stanford.edu', 18)


#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now

shutter = ch(digitalOut,3)
motBlowAway = ch(digitalOut,2)
#cameraTrigger=ch(digitalOut,0)
takeImage=ch(camera,0)

TA2 = ch(fastAnalogOut, 0)
TA3 = ch(fastAnalogOut, 1)
quadCoil = ch(fastAnalogOut, 1)
current1530 = ch(fastAnalogOut6,0)
aomSwitch0 = ch(dds, 0)
#repumpVCO=dev('ADF4360-0', 'eplittletable.stanford.edu', 0)
#coolingVCO=dev('ADF4360-6', 'eplittletable.stanford.edu', 3)
wavelength1530=ch(wavemeter, 0)
#power1530 = ch(wavemeter, 1)
absoptionLightFrequency = ch(spectrumAnalyzer, 0)
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

    ##  Digital Out holdoff  ##
    dtDigitalOutHoldoff = 11*us

    #Initialization Settings
    tStart =dtDigitalOutHoldoff + dtShutterOpenHoldOff

    ## throwaway image settings ##
#    tThrowaway = tStart
#    filename1 = 'throwaway image'
#    description1 = 'throwaway image'

    #AOM settings
#    absorptionFreq = 1067 
#    aomFreq0 = absorptionFreq / 8
    aomFreq0 = 110
#    aomAmplitude0 = 30
    aomAmplitude0 = probeIntensity
    aomHoldOff = 10*us

    ## TA Settings ##
    voltageTA2 = 1.4
    voltageTA3 = 1.5
    tTAOn = tStart + 100*ms
    dtMOTLoad = motLoadTime*ms
    tTAOff =  tTAOn + dtMOTLoad 

    ## Quad Coil Settings ##
    quadCoilVoltage = 3.01

    ## 1530 current settings ##
#    voltage1530 = 0.88

    ## Imaging Settings ##
    dtDriftTime = driftTime*ms   
   # cropVector = (337,555,20)
    cropVector = (317,535,41,41)

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
    tShutterOpen = tStart - dtShutterOpenHoldOff
    tShutterClose = t1530Off - dtShutterCloseHoldOff


    ## End of Sequence Settings ##
#    tQuadCoilEndOfSequence = tDarkBackground + tWait
#    t1530EndOfSequence = tDarkBackground + 2*tWait - dtShutterCloseHoldOff
    tTAEndOfSequence = tDarkBackground +2*ms

    #################### events #######################

#    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 30*us, "Play" )

#    meas(takeImage, tThrowaway, (expTime,description1),'picture')                #take throwaway image
    event(TA2, tStart, 0)    # TA off MOT dark to kill any residual MOT
    event(TA3, tStart, 0)    # TA off
    event(current1530, tStart, voltage1530)    #1530 light on

    event(aomSwitch0,tStart, (aomFreq0, 0 ,0)) # AOM is off, so no imaging light
    event(motBlowAway, tStart, 0)                 #set cooling light to 10 MHz detuned via RF switch
    event(shutter,tStart - dtShutterOpenHoldOff, 1)

    meas(wavelength1530, tStart)
    meas(absoptionLightFrequency, tStart)
#    meas(power1530,1*s)

    ## Load the MOT ##    
    event(TA2, tTAOn, voltageTA2)                   # TA on
    event(TA3, tTAOn, voltageTA3)                   # TA on
    event(TA2, tTAOff, 0)    # TA off
    event(TA3, tTAOff, 0)    # TA off
#
#    ## blast the mot ##
#    event(aomSwitch0, tTAOff, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light
#    event(aomSwitch0, tTAOff + 2500*us, (aomFreq0, 0, 0)) #turn off absorbtion light
#    event(motBlowAway, tTAOff - 400*us, 1) #switch to on resonance light
#    event(motBlowAway, tTAOff, 0) #switch back to detuned cooling light

    ##Turn off 1530 in preparation for imaging##
#    event(shutter, tAomOn - holdoff1530*us- dtShutterOpenHoldOff, 0)

    ## Take an absorbtion image ##
    event(aomSwitch0, tAomOn, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light
    event(aomSwitch0, tAomOn + dtAbsorbtionLight, (aomFreq0, 0, 0)) #turn off absorbtion light

    meas(takeImage, tAbsorptionCamera, (expTime, description2, filename, cropVector))                #take absorption image

    ## Take an abosorbtion calibration image after the MOT has decayed away ##

    event(aomSwitch0, tAomCalibration, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light
    event(aomSwitch0, tAomCalibration + dtAbsorbtionLight, (aomFreq0, 0, 0)) #turn off absorbtion light 

    meas(takeImage, tCalibrationCamera, (expTime,description3,filename,cropVector))                #take absorption image

    ## Take a dark background image ##
    meas(takeImage, tDarkBackground, (expTime,description4,filename,cropVector))                #take absorption image

    event(TA2, tTAEndOfSequence, voltageTA2)
    event(TA3, tTAEndOfSequence, voltageTA3)
    event(current1530, tTAEndOfSequence, voltage1530)

#    event(aomSwitch0, tTAEndOfSequence, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light 
#    event(current1530, t1530EndOfSequence, voltage1530)
#    event(quadCoil, tQuadCoilEndOfSequence, quadCoilVoltage)
#    event(ch(repumpVCO, 1), tTAEndOfSequence, "-6 dBm")
#    event(ch(repumpVCO, 0), tTAEndOfSequence + 10*ms, 2562)
#    event(shutter, tTAEndOfSequence, 1)                             #1530 shutter open

  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
