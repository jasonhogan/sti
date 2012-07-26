include('motFunction.py')


def depumpMOT(tStart, pumpingTime = 100*us, useCoolingTransition = False):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime    

    #################### events #######################

    event(repumpVariableAttenuator, tStart - 10*us, 0)
    event(repumpSwitch, tStart - 10*us, 0)
    event(motFrequencySwitch, tStart, 0) # turn on all cooling modulation
    if (not useCoolingTransition):
        event(depumpSwitch, tStart, 1) # pump into F=1
    else:
        event(depumpSwitch, tStart, 0) # cycle on F=2 to F'=3

    event(cooling87Shutter, tStart - 5*ms, 1)
    event(repumpShutter, tStart - 5*ms + 10*us, 0)

#    time = turnMOTLightOn(tStart)
#    time = turnMOTLightOff(tTAOff)
    time = turn3DZLightOn(tStart)
#    event(TA7, tStart, ta7MotVoltage)                             # z-Axis seed TA on
#    event(TA4, tStart, ta4DepumpVoltage)                             # z-Axis TA on                                           
#    event(zAxisRfSwitch, tStart, 1)
    time = turn3DZLightOff(tTAOff)

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


    event(motFrequencySwitch, tTAOff, 1) # turn off cooling modulation
    event(depumpSwitch, tTAOff, 0) # turn off depump
    
    event(repumpShutter, tTAOff, 1)

    return tTAOff


def depumpMOTA(tStart, pumpingTime = 100*us):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime    

    #################### events #######################

    event(repumpVariableAttenuator, tStart - 10*us, 0)
    event(repumpSwitch, tStart - 10*us, 0)
    event(motFrequencySwitch, tStart, 0) # turn on all cooling modulation
    event(depumpSwitch, tStart, 1) # pump into F=1

    event(cooling87Shutter, tStart - 5*ms, 1)
#    event(repumpShutter, tStart - 5*ms + 10*us, 0)

#    time = turnMOTLightOn(tStart)
#    time = turnMOTLightOff(tTAOff)
    time = turn3DZLightOn(tStart)
#    event(TA7, tStart, ta7MotVoltage)                             # z-Axis seed TA on
#    event(TA4, tStart, ta4DepumpVoltage)                             # z-Axis TA on                                           
#    event(zAxisRfSwitch, tStart, 1)
    time = turn3DZLightOff(tTAOff)

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


    event(motFrequencySwitch, tTAOff, 1) # turn off cooling modulation
    event(depumpSwitch, tTAOff, 0) # turn off depump
    
    event(repumpShutter, tTAOff, 1)

    return tTAOff


def microwavePulse(tStart, pulseTime = 500*us):
    event(sixPointEightGHzSwitch, tStart, 1)
    event(rfKnifeAmplitude,tStart, 0)
   
    timeMicrowaveOff = tStart + pulseTime
   
    event(sixPointEightGHzSwitch, timeMicrowaveOff, 0)
    
    return timeMicrowaveOff
