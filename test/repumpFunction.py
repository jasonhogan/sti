include('motFunction.py')


def repumpMOT(tStart, pumpingTime = 500*us):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime    

    #################### events #######################

    event(repumpFrequencySwitchX,  tStart, 0)                 # turn on repump
    event(repumpVariableAttenuator, tStart - 100*us, 10)
    event(motFrequencySwitch, tStart, 1) # turn off all cooling modulation

    time = turnMOTLightOn(tStart)
    time = turnMOTLightOff(tTAOff)


    event(motFrequencySwitch, tTAOff, 0) # turn on cooling modulation

    return tTAOff
