def MOT(tStart, rb87 = True, rb85 = False, dtMOTLoad=250*ms, leaveOn=False, tClearTime=100*ms, cMOT=True, dtSweepToCMOT=20*ms, cmotQuadCoilCurrent = 7, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 7, dtCMOT = 0*ms, powerReduction = 0.5, CMOT87Frequency = 220, CMOT85Frequency = 115, dtNoRepump = 20*ms, repumpAttenuatorVoltage = 10, cmotCurrentRampRate = 1):
    
    ## TA Settings ##
    tTAOn = tStart+tClearTime
    tTAOff =  tTAOn + dtMOTLoad


    #CMOT Settings

    ## Quad Coil Settings ##
    
    quadCoilHoldOff = 0*ms
    tQuadCoilOff = tTAOff - quadCoilHoldOff

    

    #################### events #######################

    tResetMOT = tStart

    if(tClearTime > 0) :
        ## Clear away any atoms  
       turnMOTLightOff(tResetMOT + 1.1*us + 2.5*ms)    # 3D TAs off

    # Initialize MOT frequency switches

#    event(ch(dds,2),tResetMOT + 60*us, (100,100,0))        # TEMPORARY -- source for the 200 MHz AOM (uses RF doubler)
    print "Clear Time "+str(tClearTime)
    event(ch(dds,1), tResetMOT + 20*us, (dds87MotFrequency, 100, 0) )
    event(cooling85DDS, tResetMOT + 30*us, (dds85MotFrequency, 100, 0) )
    event(braggAOM1, tResetMOT + 40*us, (100, 100, 0))
    event(braggAOM2, tResetMOT + 60*us, (100, 100, 0))
#    event(rf85DDS, tResetMOT + 70*us, (20, 0, 0))   
#    event(motFrequencySwitch,  tResetMOT, 0)                       # set cooling light to 10 MHz detuned via RF switch
#    event(depumpSwitch, tResetMOT, 0) # switch off depumper

    event(repumpVariableAttenuator, tResetMOT + 20*us, 4) # set repump variable attenuator to max transmission
    event(repumpSwitch, tResetMOT + 10*us, 1)

    # Initialize Quadropole Coil
#    setQuadrupoleCurrent(tResetMOT, desiredCurrent = 0, applyCurrentRamp = False, usePrecharge = False)
#    event(sfaOutputEnableSwitch, tResetMOT, 0)
    setQuadrupoleCurrent(tResetMOT, current = 0)
    event(quadrupoleOnSwitch, tResetMOT, 1)
    event(quadrupoleChargeSwitch, tResetMOT, 0)

    # Turn off stray light sources
    event(probeLightShutter, tResetMOT, 0)         #close probe light shutter
    event(TA4, tResetMOT, ta4OffVoltage)                            #turn off TA4
    event(TA5, tResetMOT, ta5OffVoltage)                            #turn off TA5


    # Initialize Lattice switches
    event(latticeShutter, tResetMOT, 0)
    event(ta7MOTShutter, tResetMOT, 1)
    event(ta7LatticeShutter, tResetMOT, 0)
    event(motZShutter, tResetMOT, 1)

    if(dtMOTLoad <= 0) :    # Load time must be greater than zero
        return tResetMOT

    ## Load the MOT ##  
    setQuadrupoleCurrent(tTAOn+20*us, current = motQuadCoilCurrent)
#    setQuadrupoleCurrent(tTAOn, desiredCurrent = motQuadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 0)
#    rampQuadrupoleCurrent(startTime = tTAOn, endTime = tTAOn+10*ms, startCurrent = 0, endCurrent = motQuadCoilCurrent, numberOfSteps = 10)

    event(quadrupoleOnSwitch, tTAOn, 1)
#    event(sfaOutputEnableSwitch, tTAOn + 100*us, 1)
#    event(cooling87Shutter, tTAOn - 5*ms, 1)

# KRISH MESSED THIS UP! FIX IT!
#####################################################   
 
    if (InterleavedMOT) :  ###
        event(cooling87AOMvca, tTAOn - 5*ms-10*us, cooling87AOMvcaMaxVal)
        event(cooling87Shutter, tTAOn - 5*ms, 1) ###
        event(cooling85Shutter, tTAOn - 5*ms, 0)
        
    else :
        if (rb87) :
            event(cooling87AOMvca, tTAOn - 5*ms-10*us, cooling87AOMvcaMaxVal)
            event(cooling87Shutter, tTAOn - 5*ms, 1)
        else :
            event(cooling87Shutter, tTAOn - 5*ms, 0)

        if (rb85) :
            event(cooling85Shutter, tTAOn - 5*ms, 5)
        else :
            event(cooling85Shutter, tTAOn - 5*ms, 0)

    event(repumpShutter, tTAOn - 5*ms + 10*us, 1)
    turn2DMOTLightOn(tTAOn)                        #ensure that 2D light is on
    turnMOTLightOn(tTAOn)                            #TAs on

    if(InterleavedMOT) :
        event(cooling87AOMvca, tTAOn + fractional87Load*dtMOTLoad -5*ms, cooling87AOMvcaLowVal)
        event(cooling85Shutter, tTAOn + fractional87Load*dtMOTLoad, 5)
        event(cooling87AOMvca, tTAOn + 1*dtMOTLoad -5*ms, cooling87AOMvcaMaxVal)
        
#    time = repumpMOT(tTAOff - dtCMOT - dtSweepToCMOT - 3*ms, pumpingTime = 200*us, ta4RepumpVoltage=1.00*TA4RepumpVoltage)
    
    if(leaveOn) :
        event(cooling87Shutter, tTAOn + 20*ms, 1)
        event(cooling85Shutter, tTAOn + 20*ms, 5)
        
        return tTAOn

    if(cMOT) :
        ## switch to a CMOT ##
#        setQuadrupoleCurrent(tTAOff - quadCoilHoldOff - dtCMOT - dtSweepToCMOT, desiredCurrent = cmotQuadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = motQuadCoilCurrent, rampRate = cmotCurrentRampRate)
               
        event(cooling87AOMvca, tTAOff - dtCMOT - dtSweepToCMOT, cooling87AOMvcaMaxVal)
        turn2DMOTLightOff(tTAOff - dtCMOT - dtSweepToCMOT - dtShutterBuffer)                          # buffer to ensure light is off before cMOT starts

#        event(ch(dds,1), tTAOff - dtCMOT - dtSweepToCMOT, ((dds87MotFrequency, CMOTFrequency, dtSweepToCMOT), 100, 0) )
#        if (InterleavedMOT) :
#            event(ch(dds,1), tTAOff - dtCMOT - dtSweepToCMOT, ((dds87MotFrequency, CMOT87Frequency, dtSweepToCMOT), 100, 0) )
#            event(cooling85DDS, tTAOff - dtCMOT - dtSweepToCMOT + 10*us, ((dds85MotFrequency, CMOT85Frequency, dtSweepToCMOT), 100, 0)
#            else :

        if(InterleavedMOT) :  ###
            event(ch(dds,1), tTAOff - dtCMOT - dtSweepToCMOT, ((dds87MotFrequency, CMOT87Frequency, dtSweepToCMOT), 100, 0) )
            event(cooling85DDS, tTAOff - dtCMOT - dtSweepToCMOT + 10*us, ((dds85MotFrequency, CMOT85Frequency, dtSweepToCMOT), 100, 0) )    
                
        else :
            if (rb87):
                event(ch(dds,1), tTAOff - dtCMOT - dtSweepToCMOT, ((dds87MotFrequency, CMOT87Frequency, dtSweepToCMOT), 100, 0) )
        
            if (rb85):
                event(cooling85DDS, tTAOff - dtCMOT - dtSweepToCMOT + 10*us, ((dds85MotFrequency, CMOT85Frequency, dtSweepToCMOT), 100, 0) )
                


#        event(ch(dds,1), tTAOff - dtCMOT+20*us, (CMOTFrequency, 50, 0) )


#        voltageSweep(channel = TA3, startTime = tTAOff - dtCMOT - 1.1*us-dtSweepToCMOT, sweepTime = dtSweepToCMOT, startVoltage = voltageTA3, stopVoltage = 1.6*powerReduction, numberOfEvents = 10)                 #voltageTA3=1.6
#        voltageSweep(channel = TAZ, startTime = tTAOff - dtCMOT-dtSweepToCMOT, sweepTime = dtSweepToCMOT, startVoltage = taZMotVoltage, stopVoltage = (0.25+(.15*powerReduction)), numberOfEvents = 10, eventFunc = setTACurrent)    #powerReduction*ta4MotVoltage        ##ta4MotVoltage=0.4

        if(dtNoRepump > 0) :
            event(repumpVariableAttenuator, tTAOff - dtNoRepump, repumpAttenuatorVoltage)
            event(repumpSwitch, tTAOff - dtNoRepump, 0)

#            event(repumpVariableAttenuator, tTAOff - dtNoRepump + 15*ms, repumpAttenuatorVoltage)

#4/12/11 -- turn off TAs outside this function.  Reset switches at beginning
    ## finish loading the MOT
#    turnMOTLightOff(tTAOff)        #TAs off
#    event(depumpSwitch, tTAOff, 0)  # reset depump switch
#    event(motFrequencySwitch, tTAOff, 0) # turn on cooling RF modulation

    return tTAOff

#
#def turn3DZLightOn(tTurnOn, ta4PowerScale = 1.0, ta4CurrentHoldoffTime = 20*us):
##    event(finalZShutter, tTurnOn - 5*ms, 1)
##    event(TA7, tTurnOn - 22*us, ta7MotVoltage)                             # z-Axis seed TA on
##    setTACurrent(TAZ, tTurnOn - ta4CurrentHoldoffTime, taZMotVoltage*ta4PowerScale)                             # z-Axis TA on                                           
##    event(zAxisRfSwitch, tTurnOn, 1)
#    return tTurnOn;

def turn3DZLightOn(tTurnOn, ta4Voltage = taZMotVoltage, ta4CurrentHoldoffTime = 20*us):
    event(finalZShutter, tTurnOn - 5*ms, 1)
#    event(TA7, tTurnOn - 22*us, ta7MotVoltage)                             # z-Axis seed TA on
    #setTACurrent(TAZ, tTurnOn - ta4CurrentHoldoffTime, ta4Voltage)                             # z-Axis TA on                                           
    #event(zAxisRfSwitch, tTurnOn, 1)
#    event(taZAgiltronSwitch, tTurnOn + 1.1*us, 0)
    return tTurnOn;

def turn3DZLightOnImaging(tTurnOn, ta4Voltage):
    event(finalZShutter, tTurnOn - 5*ms, 1)
    event(motFrequencySwitch, tTurnOn, 0) # turn on all cooling modulation
#    event(TA7, tTurnOn, ta7MotVoltage)                             # z-Axis seed TA on
    #setTACurrent(TAZ, tTurnOn, ta4Voltage)                             # z-Axis TA on                                           
    #event(zAxisRfSwitch, tTurnOn, 1)
#    event(taZAgiltronSwitch, tTurnOn + 1.1*us, 0)
    return tTurnOn;


def turn3DXYLightOn(tTurnOn):                         
#    event(TA8, tTurnOn+10*us, voltageTA8)
    event(ta3SeedShutter, tTurnOn - 3*ms, 1)        # xy-axes on
    return tTurnOn;

def turnRFModulationOn(tTurnOn):
    event(repumpSwitch, tTurnOn, 1)                                # for faster switching, turn on repump and cooling modulation
    event(motFrequencySwitch, tTurnOn, 0)                      # turn on all cooling modulation
    event(rb85FreqSwitch, tTurnOn + 15*us, 0)                      # turn on all cooling modulation
    event(cooling85Switch, tTurnOn + 25*us, 0)

def turnMOTLightOn(tTurnOn):
##    event(TA2, tTurnOn - 2.2*us, voltageTA2)                   # TA on; now in 2DMOT function
##    event(TA3, tTurnOn - 1.1*us, voltageTA3)                   # TA on; now in 2DMOT function
#    event(TA7, tTurnOn, ta7MotVoltage)                             # z-Axis seed TA on
#    event(TAZ, tTurnOn, taZMotVoltage)                             # z-Axis TA on                                           
#    event(ta3SeedShutter, tTurnOn - dtShutterBuffer, 1)        # xy-axes on
#    event(zAxisRfSwitch, tTurnOn, 1)
##    event(braggAOM1, tTurnOn, braggAOM1MOT)
##    event(zAxisAom, tTurnOn - 100*us, zAxisAomMot)
    turn3DZLightOn(tTurnOn, taZMotVoltage)
    turn3DXYLightOn(tTurnOn)
    turnRFModulationOn(tTurnOn)
    return tTurnOn;


def turn3DZLightOff(tTurnOff):
#    event(TA7, tTurnOff + 3.3*us, ta7OffVoltage)                    # z-Axis seed TA off
#    event(finalZShutter, tTurnOff , 0)
#    setTACurrent(TAZ, tTurnOff - 8*us, ta4OffVoltage)                #z-axis TA off -- AFS increased from 0V to ta4OffVoltage
    #event(zAxisRfSwitch, tTurnOff + 0*us, 0)
    event(repumpSwitch, tTurnOff - 4.5*us, 0)                               
    event(motFrequencySwitch, tTurnOff - 10*us, 1) 
    #event(ch(dds,1), tTurnOff - 8*us, (30, 0, 0))
    event(cooling85Switch, tTurnOff - 2.2*us, 5)
    event(rb85FreqSwitch, tTurnOff - 4*us, 5)
    #event(cooling85DDS, tTurnOff - 6*us, (30, 0, 0) )
    
    return tTurnOff;

def turn3DXYLightOff(tTurnOff):
    event(ta3SeedShutter, tTurnOff + 0*dtShutterBuffer - 3.3*ms, 0)   #-2.5*ms   #xy-axes off
    turn2DMOTLightOff(tTurnOff - 3.3*ms)
    return tTurnOff;

def turnRFModulationOff(tTurnOff):
#    event(repumpSwitch, tTurnOff - 5*us, 0)                                # for faster switching, turn off repump and cooling modulation
#    event(motFrequencySwitch, tTurnOff - 10*us, 1)                      # turn off all cooling modulation #- 10*us
    event(rb85FreqSwitch, tTurnOff - 15*us, 5)
    event(cooling85Switch, tTurnOff - 20*us, 5)

def turnMOTLightOff(tTurnOff):
##    event(TA2, tTurnOff + 2.2*us, 0)                   # TA off; now in 2DMOT function
##    event(TA3, tTurnOff + 1.1*us, 0)                   # TA off; now in 2DMOT function
#    event(TA7, tTurnOff + 3.3*us, 0)                    # z-Axis seed TA off
##    if(tTurnOff != 2100210000):
#    event(TAZ, tTurnOff, ta4OffVoltage)                #z-axis TA off                                           ###COMMENTED SMD - AFS increased from 0V to ta4OffVoltage
#    event(ta3SeedShutter, tTurnOff + dtShutterBuffer, 0)    #xy-axes off
#    event(zAxisRfSwitch, tTurnOff, 0)
##    event(braggAOM1, tTurnOff, braggAOM1Off)
##    event(zAxisAom, tTurnOff + 100*us, zAxisAomOff)
    turn3DZLightOff(tTurnOff)
    turn3DXYLightOff(tTurnOff)
    turnRFModulationOff(tTurnOff)
    return tTurnOff;

def turn2DMOTLightOn(tTurnOn):
#    event(TA2, tTurnOn - 2.2*us, voltageTA2)                   # 2D seed TA on
    event(TA3, tTurnOn - 1.1*us, voltageTA3)                   # 2D light TA on
    event(twoDMOTShutter, tTurnOn - dtShutterBuffer, 1)
    return tTurnOn;

def turn2DMOTLightOff(tTurnOff):
#    event(TA2, tTurnOff + 2.2*us, 0)                   # 2D seed TA off
    event(TA3, tTurnOff - 1.1*us, 0)                   # 2D light TA off
    event(twoDMOTShutter, tTurnOff - 2.5*ms, 0)
    return tTurnOff;

### Generic Functions ###

def voltageSweep(channel, startTime, sweepTime = 1.0*ms, startVoltage = 1.0, stopVoltage = 0.0, numberOfEvents = 10, eventFunc = event):
    deltaTime = sweepTime / numberOfEvents
    deltaVoltage = (stopVoltage - startVoltage) / numberOfEvents
    
    for i in range(0, numberOfEvents):
        eventFunc(channel, startTime + deltaTime*i, startVoltage + deltaVoltage*i)

    endTime = startTime + sweepTime
    return endTime

def setTACurrent(channel, time, current) :
    if(current > 2.0*1.75):
        print 'Requested current = ' + str(current) + '. Max current is set to 2.0*1.75'
        print 1/0
    event(channel, time, current)

def setzAxisAomVCA(time, voltage) :
    if((voltage > 3) or (voltage < 0)):
        print 1/0
    event(zAxisAomVCA, time, voltage)

def fullDarkZ(time):
    event(ramanPulseGate, time, 1)
    event(finalZShutter, time, 0)   

def endFullDarkZ(time):
    event(finalZShutter, time, 1)  
