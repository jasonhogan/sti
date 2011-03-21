from stipy import *

include('channels.py')
include('motFunction.py')
include('andorCameraFunctions.py')
include('repumpFunction.py')
include('depumpFunction.py')

setvar('desc', "Simple absorbtion image - MOT - 1800 MHz probe")

#setvar('imageCropVector',(514, 393 ,250))
setvar('imageCropVector',(500, 500, 490))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 0.02*ms)

setvar('MOTLoadTime', 2*s )

setvar('realTime', False)

# Global definitions

t0 = 2*ms


event(probeLightRFSwitch, t0, probeLightOff)             # AOM is off, so no imaging ligh
event(probeLightShutter, t0+1*ms, 0)

#event(opticalPumpingBiasfield, t0 - 10*us, 0) # turn off optical pumping bias field




#### Make a mot ####
time = t0 + 100*ms

setvar('varCMOTCurrent', 8)

time = MOT(time, tClearTime=100*ms, cMOT = False, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 1*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 8, dtCMOT = 20*ms, powerReduction = 1.0, CMOTFrequency = 205, dtNoRepump = 0*ms, repumpAttenuatorVoltage = 0)

#time = depumpMOT(time + 10*us, pumpingTime = 1000*us)

#time = time + 100*ms

tOff = time + 10*us
setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
event(quadrupoleOnSwitch, tOff, 0)


# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)



#### Drift ###
time = time + dtDriftTime

#### repump out of F = 1' #####
#time = repumpMOT(time + 10*us, pumpingTime = 1000*us)

#andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
#camera = ch(andorCamera, 0)
#takeFluorescenceImage(time)

##Image
dtDeadMOT = 1*s

if(realTime) : 
         ## Take an absorbtion image using Andor Solis Software ##
    time = takeSolisSoftwareAbsorptionImage (time, 75*us, dtAbsorbtionLight = 25*us)
    
else : 
        ### Andor Camera ###
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    print time
    time = takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)

      
    ## Turn on MOT steady state

tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False)    # turn MOT back on

#event(ch(digitalOut, 4), time + 4*s, 0)








