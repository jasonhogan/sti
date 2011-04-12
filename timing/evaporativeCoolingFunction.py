include('microwaveKnifeFunction.py')
include('approximateExponentialSweep.py')
include("bluePlugShutterFunction.py")
from math import pow


#def evaporate(tStart, dtHold = 1*s, fullMagneticTrapCurrent = 300, cmotCurrent = 45, usePreCharge = False, chargeCurrent = 45, rapidOff = True, dischargeCurrent = 45, makeRfCut = True, usePlug = True, rampNumber = 6):

def evaporate(tStart, dtHold = 1*s, rampNumber = 6):

    ## Quad Coil Settings ##
    
    #################### events #######################
    internalTime = tStart

#    #### Snap On Mag Field ####
#    
#
##    if (usePreCharge) :
##        event(quadrupoleChargeSwitch, internalTime, 1)
##        setQuadrupoleCurrent(internalTime + 500*us, chargeCurrent, False, False, 0)
##        event(quadrupoleChargeSwitch, internalTime + 0.15*ms, 0) # was 5*ms before we started monkeying with it (i.e. for 45A fast turn on)
#
#
#    ### Ramp up the mag field #################################################
#    timeAtEndOfRamp = setQuadrupoleCurrent(internalTime + 1*ms, fullMagneticTrapCurrent, True, False, chargeCurrent, rampRate = 1)
#    
#    if(usePlug):
#        openBluePlugShutter(internalTime)
#    else:
#        closeBluePlugShutter(internalTime)    
#
#
#    internalTime = timeAtEndOfRamp

#    event(quadCoilShuntSwitch, internalTime, 0)
    ###########################################################################


    #### Evaporate ############################################################
#    if (makeRfCut) :  # No need for this; assume that you won't call evaporate if you don't want to evaporate

    dtCut = dtHold

    ### Turn microwave horn on during evaporation ######
    event(sixPointEightGHzSwitch, internalTime, 1)
    event(sixPointEightGHzSwitch, internalTime + dtCut, 0)

    ddsRbfreq = ddsRbResonanceFreq

    evapTime = internalTime
        
    ###  Ramp 1 ###########################################
    if (rampNumber >= 1) :
        event(rfKnifeAmplitude,evapTime +  10*us, vca1)
        event(ddsRfKnife, evapTime, (approximateExponentialSweep(dt = dtRamp1, fStart = f0 + ddsRbfreq, fStop = f1 + ddsRbfreq, numberOfSteps = 10, tcFactor = 1.00), 25, 0))
        tEnd = evapTime + dtRamp1
    #######################################################

    ### Ramp 2 ############################################
    if (rampNumber >= 2) :
        evapTime = tEnd + 5*ms
        event(rfKnifeAmplitude, evapTime - 4*ms, vca2)
        event(ddsRfKnife, evapTime, (approximateExponentialSweep(dt = dtRamp2, fStart = f1 + ddsRbfreq, fStop = f2 + ddsRbfreq, numberOfSteps = 10, tcFactor = 1.00), 25, 0))
        tEnd = evapTime + dtRamp2
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

#        ### 'Ramp 4' Hold ###################################
#        evapTime = evapTime + dtRamp3 + 30*ms
#        event(rfKnifeAmplitude, evapTime, 0)
#        event(ddsRfKnife, evapTime, (f2 + ddsRbfreq, 0, 0))
#        tEnd = evapTime + dtRamp4
#        ###################################################

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

#### Ramp 6 Hold ###########################################
#        evapTime = tEnd + 9.37*ms
#        event(rfKnifeAmplitude, evapTime, 0)
#        event(ddsRfKnife, evapTime, (f5 + ddsRbfreq, 0, 0))
#        tEnd = evapTime + dtRamp 6
###########################################################

    
#    timeSpacing = 100*ms
#    numberSloshes = (dtHold - 10*ms) / (timeSpacing)
#    currentCurrent = fullMagneticTrapCurrent
#    newTime = internalTime + 10*ms
#
#    for i in range (0, numberSloshes):
#        newCurrent = currentCurrent + 15*pow(-1, i)
#        setQuadrupoleCurrent(newTime + i*timeSpacing, newCurrent, True, False, currentCurrent)
#        currentCurrent = newCurrent

#    internalTime = internalTime + dtHold + 20*ms
#
##    event(quadCoilShuntSwitch, internalTime, 1)
#
#    #### Ramp down the mag field ####
#    timeAtEndOfRamp = setQuadrupoleCurrent(internalTime + 1*ms, dischargeCurrent, True, False, fullMagneticTrapCurrent, rampRate = 1)
##    event(ddsRfKnife, timeAtEndOfRamp, (180, 0, 0))
#    event(rfKnifeAmplitude, timeAtEndOfRamp - 0.1*ms, 0)
#
#    internalTime = timeAtEndOfRamp
#
##    closeBluePlugShutter(internalTime - 10*ms)
#
#    #### Snap off the mag field ####
#    tOff = internalTime
#    if (rapidOff) : 
#        setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
#        event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
#        event(quadrupoleOnSwitch, tOff, 0)
#
#    
#
#
#    #### Dummy edge to give charge capacitor time to recharge ####
##    endOfChargeTime = internalTime + 99.1*s
##    event(ch(digitalOut, 4), endOfChargeTime - 500*us, 1)
##    event(ch(digitalOut, 4), endOfChargeTime + 1*ms, 0)
    

#    return tOff

    event(rfKnifeAmplitude, tEnd, 0)
    return tEnd