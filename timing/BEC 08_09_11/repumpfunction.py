include('motFunction.py')


def repumpMOT(tStart, pumpingTime = 500*us):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime +0*us    #6/2/2011 --- Added 10*us  #REMOVED SMD

    #################### events #######################

    event(repumpVariableAttenuator, tStart - 100*us, 10)
    event(motFrequencySwitch, tStart, 1) # turn off all cooling modulation
#    event(depumpSwitch, tStart+1*us, 0) # turn off depump modulation                ##COMMENTED SMD

#    ta4MotVoltage = 0.6

    time = turnMOTLightOn(tStart+0*us)
    time = turnMOTLightOff(tTAOff)

#    ta4MotVoltage = 0.4

    event(motFrequencySwitch, tTAOff, 0) # turn on cooling modulation

    return tTAOff


def microwaveRepump(tStart, pumpingTime = 500*us):

    deltaFreq=2
    event(ddsRfKnife, tStart-2*ms, (ddsRbResonanceFreq-deltaFreq, ddsRbResonanceFreq-deltaFreq, (pumpingTime + 3*ms)/s))
    event(rfKnifeAmplitude, tStart-2*ms, 10)

    event(sixPointEightGHzSwitch, tStart, 1)
   
    timeMicrowaveOff = tStart + pumpingTime
    event(sixPointEightGHzSwitch, timeMicrowaveOff, 0)
    
    return timeMicrowaveOff
