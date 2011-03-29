include('motFunction.py')


def depumpMOT(tStart, pumpingTime = 100*us):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime    

    #################### events #######################

    event(repumpVariableAttenuator, tStart - 100*us, 0)
    event(motFrequencySwitch, tStart, 0) # turn on all cooling modulation
    event(depumpSwitch, tStart, 1) # pump into F=1

    time = turnMOTLightOn(tStart)
    time = turnMOTLightOff(tTAOff)

    event(motFrequencySwitch, tTAOff, 0) # turn on cooling modulation
    event(depumpSwitch, tTAOff, 0) # turn off depump

    return tTAOff
