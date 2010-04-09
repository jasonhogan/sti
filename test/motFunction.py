


def MOT(tStart, dtMOTLoad=250*ms, leaveOn=False, tClearTime=100*ms, cMOT=True, dtCMOT=20*ms):

    ## TA Settings ##
    tTAOn = tStart+tClearTime
    tTAOff =  tTAOn + dtMOTLoad
    voltageTA2 = 1.45
    voltageTA3 = 1.5

    #CMOT Settings
    cmotFieldMultiplier = 1

    ## Quad Coil Settings ##
    quadCoilCurrent = 0.6
    cmotQuadCoilCurrent = cmotFieldMultiplier * quadCoilCurrent
    quadCoilHoldOff = 10*ms
    tQuadCoilOff = tTAOff

    

    #################### events #######################

    tResetMOT = tStart

    if(tClearTime > 0) :
        ## Clear away any atoms  
        event(TA2, tResetMOT, 0)    # TA off MOT dark to kill any residual MOT
        event(TA3, tResetMOT, 0)    # TA off

    # Initialize MOT frequency switches
    event(motFrequencySwitch,  tResetMOT, 0)                       # set cooling light to 10 MHz detuned via RF switch
    event(repumpFrequencySwitch,  tResetMOT, 0)                 # set repump light on resonance via RF switch

    if(dtMOTLoad <= 0) :    # Load time must be greater than zero
        return tResetMOT

    ## Load the MOT ##  
    event(quadCoil, tTAOn, quadCoilCurrent)  
    event(TA2, tTAOn, voltageTA2)                   # TA on
    event(TA3, tTAOn, voltageTA3)                   # TA on

    if(leaveOn) :
        return tTAOn

    if(cMOT) :
        ## switch to a CMOT ##
        event(motFrequencySwitch, tTAOff - rfSwitchHoldOff - dtCMOT, 1)                  #set cooling light to 90 MHz detuned via RF switch
        event(repumpFrequencySwitch, tTAOff - rfSwitchHoldOff - dtCMOT, 1)            #set repump light to -17dBm via RF switch
        event(quadCoil, tTAOff - quadCoilHoldOff - dtCMOT, cmotQuadCoilCurrent)

    ## finish loading the MOT
    event(TA2, tTAOff, 0)             # TA off
    event(TA3, tTAOff, 0)             # TA off
    event(quadCoil, tTAOff, 0)      # turn off the quad coils

    if(cMOT) :
        # reset switches
        event(motFrequencySwitch, tTAOff, 0)                
        event(repumpFrequencySwitch, tTAOff, 0)         

    return tTAOff
