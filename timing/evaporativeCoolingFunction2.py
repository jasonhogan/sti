include('microwaveKnifeFunction.py')
include('approximateExponentialSweep.py')
from math import pow
 
### VCAs ######################################################################
## VCA's control RF knife ramps
## ~4.5 is off. Frequencies are in MHz.
## Power should drop and ramps should get faster as you evaporate. 
## Several ramps are defined here, but not all may be executed. See evaporativeCoolingFunction
setvar('vca1', 9.5)    #6.5
setvar('f0', 120) #120
setvar('f1', 78) #60
setvar('dtRamp1', 1*s) #4.75


setvar('dtCut2', 5*s)
setvar('vca2', 8)
setvar('f2', 64) 
#setvar('dtRamp2Sequence', 1*s)
setvar('dtRamp2', 1*s)  #1.5*s

setvar('vca3', 6.5) #5.75
setvar('f3', 15)
setvar('dtRamp3', 0.4*s) #0.6*s

setvar('vca4', 5.5)
setvar('f4', 9)
setvar('dtRamp4Sequence', 1*s)
setvar('dtRamp4', 0.15*s) #1.1*s

setvar('vca5', 5.0)
setvar('f5', 6)
setvar('dtRamp5Sequence', 1*s)
setvar('dtRamp5', 0.15*s)

setvar('vca6', 5.0)
setvar('f6', 3)
setvar('dtRamp6', 0.15*s)
###############################################################################

def evaporate(tStart, rampNumber = 6):

    if (rampNumber < 1):
        return tStart
    
    #################### events #######################
    internalTime = tStart

    ### Turn microwave horn on during evaporation ######
    event(sixPointEightGHzSwitch, internalTime, 1)

    ddsRbfreq = ddsRbResonanceFreq

    evapTime = internalTime
    tEnd = evapTime

    ###  Ramp 1 ###########################################
    if (rampNumber >= 1) :
        event(rfKnifeAmplitude,evapTime +  10*us, vca1)
        event(ddsRfKnife, evapTime, ((ddsRbfreq + f0, ddsRbfreq + f1, dtRamp1), 100, 0) )
        tEnd = evapTime + dtRamp1
    #######################################################

    ### Ramp 2 ############################################
    if (rampNumber >= 2) :
        evapTime = tEnd + 5*ms
        event(rfKnifeAmplitude, evapTime - 4*ms, vca2)
        ddsRamps2=approximateExponentialSweep2Extended(dt = dtRamp2, fStart = f1 + ddsRbfreq, fStop = f2 + ddsRbfreq, fLimit = ddsRbfreq, numberOfSteps = 10, dtStop = dtCut2)
        setvar('cutFreqStop', ddsRamps2[-1][1] - ddsRbResonanceFreq)
        event(ddsRfKnife, evapTime, (ddsRamps2, 100, 0))
        tEnd = evapTime + dtCut2
    #######################################################

    ### Ramp 3 ############################################
    if (rampNumber >= 3):
        evapTime = tEnd + 20*ms
        event(rfKnifeAmplitude, evapTime - 10*ms, vca3)
        event(ddsRfKnife, evapTime, (approximateExponentialSweep(dt = dtRamp3, fStart = f2 + ddsRbfreq, fStop = f3 + ddsRbfreq, numberOfSteps = 10, tcFactor = 1.0), 25, 0))
        tEnd = evapTime + dtRamp3
    #######################################################

    ### Ramp 4 ############################################
    if (rampNumber >= 4):
        evapTime = tEnd + 30*ms
        event(rfKnifeAmplitude, evapTime - 20*ms, vca4)
        event(ddsRfKnife, evapTime, (approximateExponentialSweep(dt = dtRamp4, fStart = f3 + ddsRbfreq, fStop = f4 + ddsRbfreq, numberOfSteps = 10, tcFactor = 1.00), 25, 0))
        tEnd = evapTime + dtRamp4
    #######################################################

    ### Ramp 5 ############################################
    if (rampNumber >= 5):
        evapTime = tEnd + 11.23*ms
        event(rfKnifeAmplitude, evapTime, vca5)
        event(ddsRfKnife, evapTime, (approximateExponentialSweep(dt = dtRamp5, fStart = f4 + ddsRbfreq, fStop = f5 + ddsRbfreq, numberOfSteps = 10, tcFactor = 1.00), 25, 0))
        tEnd = evapTime + dtRamp5
    #######################################################

    ### Ramp 6 ############################################
    if (rampNumber >= 6):
        evapTime = tEnd + 1.3*ms
        event(rfKnifeAmplitude, evapTime, vca6)
        event(ddsRfKnife, evapTime, (approximateExponentialSweep(dt = dtRamp6, fStart = f5 + ddsRbfreq, fStop = f6 + ddsRbfreq, numberOfSteps = 10, tcFactor = 1.00), 25, 0))
        tEnd = evapTime + dtRamp6
    #######################################################

    event(rfKnifeAmplitude, tEnd + .1*ms, 0)
    #### Turn off microwave horn at end of evaporation)
    event(sixPointEightGHzSwitch, tEnd + .1*ms, 0)
    return tEnd
