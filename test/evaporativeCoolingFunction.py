include('microwaveKnifeFunction.py')
include('approximateExponentialSweep.py')


def evaporate(tStart, dtHold = 1*s, fullMagneticTrapCurrent = 300, cmotCurrent = 45, usePreCharge = False, chargeCurrent = 45, rapidOff = True, dischargeCurrent = 45, makeRfCut = True):

    ## Quad Coil Settings ##
    
    #################### events #######################
    internalTime = tStart

    #### Load Magnetic Trap ####

    #### Optical Pumping ####
        
    #### Snap On Mag Field ####

    if (usePreCharge) :
        event(quadrupoleChargeSwitch, internalTime, 1)
        setQuadrupoleCurrent(internalTime + 500*us, chargeCurrent, False, False, 0)
        event(quadrupoleChargeSwitch, internalTime + 5*ms, 0)
    else :
        setQuadrupoleCurrent(internalTime, cmotCurrent, False, False, 0)

    #### Ramp up the mag field ####
    timeAtEndOfRamp = setQuadrupoleCurrent(internalTime + 1*ms, fullMagneticTrapCurrent, True, False, chargeCurrent)
    
    
    internalTime = timeAtEndOfRamp

    #### Evaporate ####
    if (makeRfCut) :
        dtCut = dtHold - 1*s
        
#        approximateExponentialSweep(tStart = internalTime, dt = dtCut, fStart = 50, fStop = 30, numberOfSteps = 5)
        event(ddsRfKnife, internalTime, ((180, 160, dtCut), 100, 0))
        event(ddsRfKnife, internalTime + dtCut, (160, 0, 0))
#        event(starkShiftingAOM, internalTime, (10, 100, 0))
#        event(starkShiftingAOM, internalTime, ((30, 10, dtHold), 100, 0))

        event(sixPointEightGHzSwitch, internalTime, 1)
        event(sixPointEightGHzSwitch, internalTime + dtCut, 0)

#        numberOfCommands = int(dtCut / (100*ms))
#        dFrequency = 3.0e+006
#        for i in range (0, numberOfCommands) :
#            event(rfKnifeFrequency, internalTime + i*dFrequency, rbResonanceFreq - 110e+006 + dFrequency * i)
    
    internalTime = internalTime + dtHold

#    event(starkShiftingAOM, internalTime, (10, 0, 0))

    #### Ramp down the mag field ####
    timeAtEndOfRamp = setQuadrupoleCurrent(internalTime + 1*ms, dischargeCurrent, True, False, fullMagneticTrapCurrent)
    event(ddsRfKnife, timeAtEndOfRamp, (180, 0, 0))

    internalTime = timeAtEndOfRamp

    #### Snap off the mag field ####
    tOff = internalTime
    if (rapidOff) : 
        setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
        event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
        event(quadrupoleOnSwitch, tOff, 0)


    #### Dummy edge to give charge capacitor time to recharge ####
#    endOfChargeTime = internalTime + 99.1*s
#    event(ch(digitalOut, 4), endOfChargeTime - 500*us, 1)
#    event(ch(digitalOut, 4), endOfChargeTime + 1*ms, 0)
    

    return tOff
