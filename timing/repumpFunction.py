include('motFunction.py')


def repumpMOT(tStart, pumpingTime = 500*us, ta4RepumpVoltage = TA4RepumpVoltage, repumpAttenuator = repumpVariableAttenuatorVoltage, opticalPumping = False):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime +0*us    #6/2/2011 --- Added 10*us  #REMOVED SMD

    #################### events #######################

#    event(ch(dds,1), tStart - 100*us, (ddsMotFrequency+80, 100, 0) )

    event(repumpShutter, tStart - 5*ms + 10*us, 1)
    event(repumpVariableAttenuator, tStart - 10*us, repumpAttenuator)
    event(repumpSwitch, tStart - 10*us, 1)

    if(not opticalPumping):
        event(cooling85Shutter, tStart - 2*ms - 10*us, 0) #7*ms
        event(cooling87Shutter, tStart - 2*ms, 0)  #7*ms instead of 5*ms so that cooling 87 can turn back on sooner at the end of this function.
    else:
        event(cooling85Shutter, tStart - 4*ms - 10*us, 0) #tStart - 1*ms - 10*us
        event(cooling87Shutter, tStart - 4*ms, 0) #-1*ms
    
    event(motFrequencySwitch, tStart, 1) # turn off all cooling modulation
    event(depumpSwitch, tStart+1*us, 0) # turn off depump modulation  
    event(cooling85DDS, tStart, (10, 0, 0)) #turn off 85 cooling modulation
    event(cooling85DDS, tStart-2*us, (10, 0, 0)) #turn off 85 cooling modulation

#    setvar('ta4MotVoltage', 0.6)

#    time = turnMOTLightOn(tStart+10*us)
#    time = turnMOTLightOff(tTAOff+10*us)

    time = turn3DZLightOn(tStart+0*us, ta4Voltage = ta4RepumpVoltage)
    time = turn3DZLightOff(tTAOff)

#    ta4MotVoltage = 0.4

    #event(motFrequencySwitch, tTAOff, 0) # turn on cooling modulation

#    event(cooling87Shutter, tTAOff-2*ms, 1)  #2*ms gotten from oscilloscope traces. Keeps cooling 87 off when you think it's off.  (~1ms from end of repump pulse)
#    event(cooling85Shutter, tTAOff-2*ms, 5) #-.002

#    event(repumpShutter, tTAOff, 0)
    if(opticalPumping):
         event(finalZShutter, tTAOff - 4*us, 0)
         event(repumpShutter, tTAOff, 0)
         event(TARepump, tTAOff, 0)
         event(TARepump, tTAOff+10*ms, voltageTARepump)

    event(repumpVariableAttenuator, tTAOff + 2*us, repumpVariableAttenuatorVoltage)
    event(cooling85DDS, tTAOff, (dds85MotFrequency, 100, 0))


    return time

def blowawayMOT(tStart, pumpingTime = 500*us, repumpAttenuator = 0, blowawayForAI = False):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime + 0*us    #6/2/2011 --- Added 10*us  #REMOVED SMD

    #################### events #######################

#    event(ch(dds,1), tStart - 100*us, (ddsMotFrequency+80, 100, 0) )

    event(TARepump, tStart - 4.5*ms, 0)
    event(repumpShutter, tStart - 4*ms - 10*us, 1) #-3*ms
    event(repumpVariableAttenuator, tStart - 2*ms - 10*us, repumpAttenuator)
    event(repumpSwitch, tStart, 0)

    event(cooling85Shutter, tStart - 4*ms - 10*us, 0)
    event(cooling87Shutter, tStart - 4*ms, 0)  #7*ms instead of 5*ms so that cooling 87 can turn back on sooner at the end of this function.
    event(motFrequencySwitch, tStart - 4*ms, 1) # turn off all cooling modulation
    event(depumpSwitch, tStart - 4*ms, 0) # turn off depump modulation
    event(f1BlowawayShutter, tStart + 5*us, 0) #this is actually an RF switch, not a shutter!
    event(f1BlowawayShutter, tStart + 5*us + pumpingTime, 1)                        

#    setvar('ta4MotVoltage', 0.6)

#    time = turnMOTLightOn(tStart+10*us)
#    time = turnMOTLightOff(tTAOff+10*us)
    
#    time = turn3DZLightOn(tStart+0*us, ta4Voltage = ta4RepumpVoltage)
    event(finalZShutter, tStart - 5*ms, 1)

    event(TARepump, tStart, voltageTARepump)
    event(TARepump, tTAOff, 0)
    time = turn3DZLightOff(tTAOff + 10*us)

#    ta4MotVoltage = 0.4

    #event(motFrequencySwitch, tTAOff, 0) # turn on cooling modulation

#    event(cooling87Shutter, tTAOff-2*ms, 1)  #2*ms gotten from oscilloscope traces. Keeps cooling 87 off when you think it's off.  (~1ms from end of repump pulse)
#    event(cooling85Shutter, tTAOff-2*ms, 5) #-.002

    event(repumpShutter, tTAOff + 4*us, 0)
    if(not blowawayForAI):
        event(finalZShutter, tTAOff, 0)

    event(repumpVariableAttenuator, tTAOff + 2*us, 0)

    event(TARepump, tTAOff + 5*ms + 2*us, voltageTARepump)

    return time



def repumpMOT85(tStart, pumpingTime = 100*us, ta4RepumpVoltage = TA4RepumpVoltage, repumpAttenuator = 4):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime +0*us    #6/2/2011 --- Added 10*us  #REMOVED SMD

    #################### events #######################

#    event(ch(dds,1), tStart - 100*us, (ddsMotFrequency+80, 100, 0) )
    
    event(repumpVariableAttenuator, tStart - 10*ms, repumpAttenuator)
    event(repumpShutter, tStart - 4*ms, 1)
    event(repumpSwitch, tStart - 5*ms, 1)
    event(motZShutter, tStart - 4*ms, 1)


    event(cooling85Shutter, tStart - 7*ms - 10*us, 0)
    event(cooling87Shutter, tStart - 7*ms, 0)  #7*ms instead of 5*ms so that cooling 87 can turn back on sooner at the end of this function.
#    event(motFrequencySwitch, tStart, 1) # turn off all cooling modulation
#    event(depumpSwitch, tStart+1*us, 0) # turn off depump modulation               

#    setvar('ta4MotVoltage', 0.6)

#    time = turnMOTLightOn(tStart+10*us)
#    time = turnMOTLightOff(tTAOff+10*us)

    time = turn3DZLightOn(tStart, ta4Voltage = ta4RepumpVoltage)
    time = turn3DZLightOff(tTAOff)

#    ta4MotVoltage = 0.4

#    event(motFrequencySwitch, tTAOff, 0) # turn on cooling modulation

#    event(cooling87Shutter, tTAOff-2*ms, 1)  #2*ms gotten from oscilloscope traces. Keeps cooling 87 off when you think it's off.  (~1ms from end of repump pulse)
#    event(cooling85Shutter, tTAOff-2*ms, 5) #-.002

    event(repumpVariableAttenuator, tTAOff + 3*ms, 4)
    event(motZShutter, tTAOff, 1)

    event(cooling85Shutter, tTAOff + 10*ms, 1)
    event(cooling87Shutter, tTAOff + 11*ms, 1)  #7*ms instead of 5*ms so that cooling 87 can turn back on sooner at the end of this function.

    return time





def microwaveRepump(tStart, pumpingTime = 500*us):

#    setvar('rabiFlopDetuningSeq',0.0)
#    setvar('rabiFlopDetuning',rabiFlopDetuningSeq)
    setvar('rabiFlopDetuning', 9)
    setvar('vcaUWaveRepump', 10)

    event(rfKnifeAmplitude, tStart, vcaUWaveRepump)
    event(sixPointEightGHzSwitch, tStart, 1)


    event(ddsRfKnife, tStart, (ddsRbResonanceFreq+rabiFlopDetuning, 100, 0))
    
    timeMicrowaveOff = tStart + pumpingTime
    
    event(sixPointEightGHzSwitch, timeMicrowaveOff, 0)
    event(rfKnifeAmplitude, time, 0)
    
    return timeMicrowaveOff


def repumpDepump(tStart, pumpingTime = 500*us):
    ## Switch Settings ##

    tTAOff = tStart + pumpingTime +0*us    #6/2/2011 --- Added 10*us  #REMOVED SMD

    #################### events #######################

#    event(ch(dds,1), tStart - 100*us, (ddsMotFrequency+80, 100, 0) )
    ### NO OFFSETS ON SHUTTERS ###
    event(repumpShutter, tStart + 10*us, 1)
    event(repumpVariableAttenuator, tStart + 20*us, 10)
    event(repumpSwitch, tStart + 30*us, 1)

    event(cooling87Shutter, tStart, 1)
    event(motFrequencySwitch, tStart, 0) # turn on all cooling modulation
    event(depumpSwitch, tStart+1*us, 1) # turn on depump modulation               

#    setvar('ta4MotVoltage', 0.6)

#    time = turnMOTLightOn(tStart+10*us)
#    time = turnMOTLightOff(tTAOff+10*us)

    time = turn3DZLightOn(tStart+0*us, ta4PowerScale = 1.00)
    time = turn3DZLightOff(tTAOff)

#    ta4MotVoltage = 0.4

    event(depumpSwitch, time+1*us, 0) # turn off depump modulation

    return tTAOff


def setF1BlowawaySwitch(time, state) :

    if(state == 'Repump') :
        event(f1BlowawayShutter, time, 1)
    elif(state=='F1Blowaway') :
        event(f1BlowawayShutter, time, 0)
    else:
        print (1/0)    #Invalid state for F1 blowaway switch

    return time

def repumpMOTForImaging(tStart, pumpingTime = 500*us, ta4RepumpVoltage = TA4RepumpVoltage):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime +0*us    #6/2/2011 --- Added 10*us  #REMOVED SMD

    #################### events #######################

#    event(ch(dds,1), tStart - 100*us, (ddsMotFrequency+80, 100, 0) )

    event(repumpShutter, tStart - 5*ms + 10*us, 1)
    event(repumpVariableAttenuator, tStart - 10*us, repumpVariableAttenuatorVoltage)
    event(repumpSwitch, tStart - 10*us, 1)

    event(cooling85Shutter, tStart - 7*ms - 10*us, 0)
    event(cooling87Shutter, tStart - 7*ms, 0)  #7*ms instead of 5*ms so that cooling 87 can turn back on sooner at the end of this function.
    event(motFrequencySwitch, tStart, 1) # turn off all cooling modulation
    event(depumpSwitch, tStart+1*us, 0) # turn off depump modulation               

#    setvar('ta4MotVoltage', 0.6)

#    time = turnMOTLightOn(tStart+10*us)
#    time = turnMOTLightOff(tTAOff+10*us)

    time = turn3DZLightOn(tStart+0*us, ta4Voltage = ta4RepumpVoltage)
    time = turn3DZLightOff(tTAOff)

#    ta4MotVoltage = 0.4

    #event(motFrequencySwitch, tTAOff, 0) # turn on cooling modulation

    return time

    
