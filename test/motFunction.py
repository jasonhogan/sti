


def MOT(tStart, dtMOTLoad=250*ms, leaveOn=False, tClearTime=100*ms, cMOT=True, dtCMOT=20*ms, cmotFieldMultiplier = 3, dtMolasses = 0*ms, rapidOff=True, quadCoilSetting = 1.8, dtFarDetuned = 0*ms):

    ## TA Settings ##
    tTAOn = tStart+tClearTime
    tTAOff =  tTAOn + dtMOTLoad
    voltageTA2 = 1.65
    voltageTA3 = 1.5

    #CMOT Settings
    

    ## Quad Coil Settings ##
    
    cmotQuadCoilCurrent = cmotFieldMultiplier * quadCoilSetting
    quadCoilHoldOff = 0*ms
    tQuadCoilOff = tTAOff - quadCoilHoldOff

    

    #################### events #######################

    tResetMOT = tStart

    if(tClearTime > 0) :
        ## Clear away any atoms  
        event(TA2, tResetMOT, 0)    # TA off MOT dark to kill any residual MOT
        event(TA3, tResetMOT, 0)    # TA off

    # Initialize MOT frequency switches
    event(motFrequencySwitch,  tResetMOT, 0)                       # set cooling light to 10 MHz detuned via RF switch
    event(repumpFrequencySwitchX,  tResetMOT, 0)                 # set repump light on resonance via RF switch
    event(cmotSwitch, tResetMOT, 0) # switch to 1156 MHz -> 90 MHz red detuned

    if(cMOT or leaveOn) :
        event(ch(vco3, 0), tResetMOT - 3*us, 1065.5 + 10 )    # detuned by -10 MHz 2->3'
        event(ch(vco0 , 0), tResetMOT+3*us + 2.023*ms, 2526)

    if(dtMOTLoad <= 0) :    # Load time must be greater than zero
        return tResetMOT

    ## Load the MOT ##  
    event(quadCoilVoltage, tTAOn, quadCoilSetting)
    event(quadCoilSwitch, tTAOn, 1)  
    event(TA2, tTAOn, voltageTA2)                   # TA on
    event(TA3, tTAOn, voltageTA3)                   # TA on

    if(leaveOn) :
        return tTAOn

    if(cMOT) :
        ## switch to a CMOT ##
        event(cmotSwitch,  tTAOff - rfSwitchHoldOff - dtCMOT - dtFarDetuned, 1) # switch to 1156 MHz -> 90 MHz red detuned
#        muteTime = 50*us
#        event(TA2, tTAOff - rfSwitchHoldOff - dtCMOT-muteTime, 0)             # TA off
#        event(TA3, tTAOff - rfSwitchHoldOff - dtCMOT-muteTime, 0)             # TA off
        event(ch(vco3, 0),  tTAOff - dtCMOT+ 0.2*ms, 1156 )    # set cooling laser CMOT detuning
#        event(ch(vco0, 1), tTAOff + 2*ms, "-13 dBm") # reduce available repump power
#        event(TA2, tTAOff - rfSwitchHoldOff - dtCMOT, voltageTA2)             # TA on - turn TAs back on after frequency switch has occurred
#        event(TA3, tTAOff - rfSwitchHoldOff - dtCMOT, voltageTA3)             # TA on - turn TAs back on after frequency switch has occurred
#        event(motFrequencySwitch, tTAOff - rfSwitchHoldOff - dtCMOT, 1)                  #set cooling light to 90 MHz detuned via RF switch
#        event(repumpFrequencySwitchX, tTAOff - rfSwitchHoldOff - dtCMOT, 1)            #set repump light to -17dBm via RF switch
        event(quadCoilVoltage, tTAOff - quadCoilHoldOff - dtFarDetuned, cmotQuadCoilCurrent)

        event(cmotSwitch,  tTAOff - rfSwitchHoldOff - dtFarDetuned + 1*us, 0) # switch to 1156 MHz -> 90 MHz red detuned

        if (rapidOff) :
            event(quadCoilSwitch, tTAOff - quadCoilHoldOff - dtMolasses, 0)      # turn off the quad coils

    ## finish loading the MOT
    event(TA2, tTAOff, 0)             # TA off
    event(TA3, tTAOff, 0)             # TA off
    if(rapidOff) :
        event(quadCoilSwitch, tQuadCoilOff, 0)      # turn off the quad coils

#    if(cMOT) :
        # reset switches
#        event(motFrequencySwitch, tTAOff, 0)                
#        event(repumpFrequencySwitchX, tTAOff, 0)         


    return tTAOff
