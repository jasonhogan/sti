


def depumpMOT(tStart, pumpingTime = 100*us):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime    

    #################### events #######################

    event(repumpFrequencySwitchX,  tStart, 1)                 # turn off repump
    event(motFrequencySwitch, tStart, 0) # turn on all cooling modulation
    event(depumpSwitch, tStart, 1) # pump into F=1

    event(TA2, tStart, voltageTA2) # TA on
    event(TA3, tStart, voltageTA3)                   # TA on
    event(TA7, tStart, ta7MotVoltage)                   # TA on

    event(TA2, tTAOff, 0)                    # TA off
    event(TA3, tTAOff, 0)                   # TA off
    event(TA7, tTAOff, 0)                   # TA off

    event(motFrequencySwitch, tTAOff, 0) # turn on cooling modulation
    event(repumpFrequencySwitchX,  tTAOff, 0)                 # turn on repump
    event(depumpSwitch, tTAOff, 0) # turn off depump

    return tTAOff
