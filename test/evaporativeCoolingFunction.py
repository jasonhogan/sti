include('microwaveKnifeFunction.py')
include('approximateExponentialSweep.py')
include("bluePlugShutterFunction.py")


def evaporate(tStart, dtHold = 1*s, fullMagneticTrapCurrent = 300, cmotCurrent = 45, usePreCharge = False, chargeCurrent = 45, rapidOff = True, dischargeCurrent = 45, makeRfCut = True):

    ## Quad Coil Settings ##
    
    #################### events #######################
    internalTime = tStart

    #### Load Magnetic Trap ####

    #### Optical Pumping ####
        
    #### Snap On Mag Field ####
#    openBluePlugShutter(internalTime)

    if (usePreCharge) :
        event(quadrupoleChargeSwitch, internalTime, 1)
        setQuadrupoleCurrent(internalTime + 500*us, chargeCurrent, False, False, 0)
        event(quadrupoleChargeSwitch, internalTime + 0.15*ms, 0) # was 5*ms before we started monkeying with it (i.e. for 45A fast turn on)
    else :
        setQuadrupoleCurrent(internalTime, cmotCurrent, False, False, 0)

    #### Ramp up the mag field ####
    timeAtEndOfRamp = setQuadrupoleCurrent(internalTime + 1*ms, fullMagneticTrapCurrent, True, False, chargeCurrent)
    
    
    internalTime = timeAtEndOfRamp

    #### Evaporate ####
    if (makeRfCut) :
        dtCut = dtHold

        ddsRbfreq = 62.5

        event(rfKnifeAmplitude, internalTime + 10*us, vca1)
#        event(ddsRfKnife, internalTime, (approximateExponentialSweep(dt = dtRamp1, fStart = f0 + ddsRbfreq, fStop = f1 + ddsRbfreq, numberOfSteps = 20, tcFactor = 1.00), 25, 0))
#        event(rfKnifeAmplitude, internalTime + dtRamp1, vca2)
#        event(ddsRfKnife, internalTime + dtRamp1 + 10*ms, (approximateExponentialSweep(dt = dtRamp2, fStart = 72.5 + ddsRbfreq, fStop = 36.25 + ddsRbfreq, numberOfSteps = 10, tcFactor = 1.00), 25, 0))
#        event(rfKnifeAmplitude, internalTime + dtRamp1 + dtRamp2 + 10*ms, vca3)
#        event(ddsRfKnife, internalTime + dtRamp1 + dtRamp2+ 20*ms, (approximateExponentialSweep(dt = dtRamp3, fStart = 36.25 + ddsRbfreq, fStop = 18 + ddsRbfreq, numberOfSteps = 10, tcFactor = 0.9), 25, 0))
#        event(rfKnifeAmplitude, internalTime + dtRamp1 + dtRamp2 + dtRamp3 + 10*ms, vca4)
#        event(ddsRfKnife, internalTime + dtRamp1 + dtRamp2 + dtRamp3 + 30*ms, (approximateExponentialSweep(dt = dtRamp4, fStart = 18 + ddsRbfreq, fStop = 10 + ddsRbfreq, numberOfSteps = 10, tcFactor = 1.00), 25, 0))


#        event(ddsRfKnife, internalTime + 2.2*s + 10*ms, ((122.5, 92.5, 1.1*s), 25, 0))
#        approximateExponentialSweepHP(internalTime, 1.5*s, fStart = 110, fStop = 109, numberOfSteps = 10, tcFactor = 1)


#        event(ddsRfKnife, internalTime + dtCut + 10*ms, (110, 0, 0))
        event(sixPointEightGHzSwitch, internalTime, 1)
        event(sixPointEightGHzSwitch, internalTime + dtCut, 0)
    
    internalTime = internalTime + dtHold + 20*ms


    #### Ramp down the mag field ####
    timeAtEndOfRamp = setQuadrupoleCurrent(internalTime + 1*ms, dischargeCurrent, True, False, fullMagneticTrapCurrent)
#    event(ddsRfKnife, timeAtEndOfRamp, (180, 0, 0))
    event(rfKnifeAmplitude, timeAtEndOfRamp - 0.1*ms, 0)

    internalTime = timeAtEndOfRamp

    #### Snap off the mag field ####
    tOff = internalTime
    if (rapidOff) : 
        setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
        event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
        event(quadrupoleOnSwitch, tOff, 0)

#    closeBluePlugShutter(internalTime - 10*ms)


    #### Dummy edge to give charge capacitor time to recharge ####
#    endOfChargeTime = internalTime + 99.1*s
#    event(ch(digitalOut, 4), endOfChargeTime - 500*us, 1)
#    event(ch(digitalOut, 4), endOfChargeTime + 1*ms, 0)
    

    return tOff
