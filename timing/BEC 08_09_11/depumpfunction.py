include('motFunction.py')


def depumpMOT(tStart, pumpingTime = 100*us):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime    

    #################### events #######################

    event(repumpVariableAttenuator, tStart - 150*us, 0)
    event(motFrequencySwitch, tStart, 0) # turn on all cooling modulation
    event(depumpSwitch, tStart, 1) # pump into F=1

    time = turnMOTLightOn(tStart)
    time = turnMOTLightOff(tTAOff)

#    event(depumpSwitch, tStart, 0) # pump into F=1
#
#    event(zAxisRfSwitch, tStart, 1)
#    event(braggAOM1, tStart, braggAOM1MOT)
#    event(ch(digitalOut, 1), tStart + 9*us, 1)
#
#    event(zAxisRfSwitch, tTAOff, 0)
#    event(braggAOM1, tTAOff, braggAOM1Off)
#    event(ch(digitalOut, 1), tTAOff + 9*us, 0)


#    event(repumpVariableAttenuator, tStart - 100*us, 10)
#    event(motFrequencySwitch, tTAOff, 1) # turn off cooling modulation
    event(depumpSwitch, tTAOff, 0) # turn off depump

    return tTAOff
