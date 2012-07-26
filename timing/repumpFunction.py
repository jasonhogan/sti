include('motFunction.py')


def repumpMOT(tStart, pumpingTime = 500*us, ta4PowerFrac = 1):

    ## Switch Settings ##

    tTAOff = tStart + pumpingTime +0*us    #6/2/2011 --- Added 10*us  #REMOVED SMD

    #################### events #######################

#    event(ch(dds,1), tStart - 100*us, (ddsMotFrequency+80, 100, 0) )

    event(repumpShutter, tStart - 5*ms + 10*us, 1)
    event(repumpVariableAttenuator, tStart - 10*us, 10)
    event(repumpSwitch, tStart - 10*us, 1)

    event(cooling87Shutter, tStart - 7*ms, 0)  #7*ms instead of 5*ms so that cooling 87 can turn back on sooner at the end of this function.
    event(motFrequencySwitch, tStart, 1) # turn off all cooling modulation
    event(depumpSwitch, tStart+1*us, 0) # turn off depump modulation               

#    setvar('ta4MotVoltage', 0.6)

#    time = turnMOTLightOn(tStart+10*us)
#    time = turnMOTLightOff(tTAOff+10*us)

    time = turn3DZLightOn(tStart+0*us, ta4PowerScale = ta4PowerFrac)
    time = turn3DZLightOff(tTAOff)

#    ta4MotVoltage = 0.4

    event(motFrequencySwitch, tTAOff, 0) # turn on cooling modulation

    event(cooling87Shutter, tTAOff-2*ms, 1)  #2*ms gotten from oscilloscope traces. Keeps cooling 87 off when you think it's off.  (~1ms from end of repump pulse)

    return tTAOff







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

