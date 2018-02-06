include('motFunction.py')

def depumpMOT(tStart, pumpingTime = 100*us, useCoolingTransition = False, ta4DepumpVoltage = TA4DepumpVoltage):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime    

    #################### events #######################

    event(repumpVariableAttenuator, tStart - 10*us, 0) #2
    event(repumpSwitch, tStart - 10*us, 0)
    event(motFrequencySwitch, tStart, 0) # turn on all cooling modulation
    event(ch(dds,1), tStart, (30, 0, 0) )  #detune 87 cooling DDS so there is no cooling light
    event(cooling85Switch, tStart - 12*us, 0)
    event(rb85FreqSwitch, tStart - 14*us, 0)

#    event(cooling85Shutter, tStart - 20*ms, 0)
    
    if (not useCoolingTransition):
        event(depumpSwitch, tStart, 1) # pump into F=1
        event(cooling85DDS, tStart, (dds85DepumpFrequency, 100, 0) ) # pump into F=2
    else:
        event(depumpSwitch, tStart, 0) # cycle on F=2 to F'=3
        event(cooling85DDS, tStart, (dds85MotFrequency, 100, 0) ) # pump into F=2

    #event(cooling87Shutter, tStart - 5*ms, 0)
    #event(cooling85Shutter, tStart - 5*ms, 0)
    
    
    event(repumpShutter, tStart - 5*ms + 10*us, 0)
    
#
#    time = turnMOTLightOn(tStart)
#    time = turnMOTLightOff(tTAOff)

#    time = turn3DZLightOn(tStart, ta4Voltage = ta4DepumpVoltage)

    event(finalZShutter, tStart - 5*ms, 1)
#    setTACurrent(TAZ, tStart - 20*us, taZMotVoltage)                                                                    
#    event(zAxisRfSwitch, tStart, 1)


    
#    event(finalZShutter, tStart - 5*ms, 1)
#    event(motFrequencySwitch, tTAOff - 10*us, 1)
#    event(motZShutter, tTAOff - 5*us, 0)          

#    event(TA7, tStart, ta7MotVoltage)                             # z-Axis seed TA on
#    event(TAZ, tStart, ta4DepumpVoltage)                             # z-Axis TA on                                           
#    event(zAxisRfSwitch, tStart, 1)

#    time = turn3DZLightOff(tTAOff)
#    event(zAxisRfSwitch, tTAOff + 0*us, 0)
#    event(motZShutter, tTAOff - 200*us, 0)
    event(cooling87Shutter, tTAOff - 200*us, 0)
    event(cooling85Shutter, tTAOff - 200*us + 10*us-5*ms, 0)
    event(finalZShutter, tTAOff - 200*us, 0)
    
    event(motFrequencySwitch, tTAOff + 2*ms, 1) #this is strange--needs depump on until the shutters close  # turn off all cooling modulation
    event(depumpSwitch, tTAOff + 8*ms, 0)

    event(cooling85Switch, tTAOff + 3*us + 2*ms, 5)
    event(rb85FreqSwitch, tTAOff + 5*us + 2*ms, 5)

#    event(cooling87Shutter, tTAOff - 200*us + 10*ms, 1) #leave the shutters open during evaporation since finalZShutter is closed
#    event(cooling85Shutter, tTAOff - 200*us +10*ms + 10*us, 5)
    

#    time = turn3DZLightOn(tStart + 10*ms, ta4PowerFrac)
#    time = turn3DZLightOff(tTAOff + 10*ms)
#    tTAOff += 10*ms

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
#    event(depumpSwitch, tTAOff, 0) # turn off depump
    
#    event(repumpShutter, tTAOff + 100*ms, 1)

    return tTAOff



def depumpMOT87(tStart, pumpingTime = 100*us, useCoolingTransition = False, ta4DepumpVoltage = TA4DepumpVoltage):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime    

    #################### events #######################

    event(repumpVariableAttenuator, tStart - 10*us, 0) #2
    event(repumpSwitch, tStart - 10*us, 0)
    event(motFrequencySwitch, tStart, 0) # turn on all cooling modulation
    event(ch(dds,1), tStart, (30, 0, 0) )  #detune 87 cooling DDS so there is no cooling light
       
    if (not useCoolingTransition):
        event(depumpSwitch, tStart, 1) # pump into F=1

    else:
        event(depumpSwitch, tStart, 0) # cycle on F=2 to F'=3
        

#    event(cooling87Shutter, tStart - 5*ms, 1)
    event(repumpShutter, tStart - 5*ms + 10*us, 0)
    event(finalZShutter, tStart - 5*ms, 1)
    


    
    event(motFrequencySwitch, tTAOff+2*ms, 1) # turn off cooling modulation
    event(depumpSwitch, tTAOff + 8*ms, 0) # turn off depump
    
    event(cooling87Shutter, tTAOff - 200*us, 0)
    event(finalZShutter, tTAOff - 200*us, 0)
#    event(repumpShutter, tTAOff + 5*ms, 1)

    return tTAOff


def pushF2Pulse(tStart, pumpingTime = 5*us, useCoolingTransition = True,  ta4PusherVoltage = 1.8):

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
    time = turn3DZLightOn(tStart, ta4Voltage = ta4PusherVoltage, ta4CurrentHoldoffTime = 1*ms)
#    event(TA7, tStart, ta7MotVoltage)                             # z-Axis seed TA on
#    event(TAZ, tStart, ta4DepumpVoltage)                             # z-Axis TA on                                           
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
#    event(TAZ, tStart, ta4DepumpVoltage)                             # z-Axis TA on                                           
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
