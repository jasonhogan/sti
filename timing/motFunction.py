def MOT(tStart, dtMOTLoad=250*ms, leaveOn=False, tClearTime=100*ms, cMOT=True, dtSweepToCMOT=20*ms, cmotQuadCoilCurrent = 7, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 7, dtCMOT = 0*ms, powerReduction = 0.5, CMOTFrequency = 220, dtNoRepump = 0*ms, repumpAttenuatorVoltage = 10, cmotCurrentRampRate = 1):

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
       turnMOTLightOff(tResetMOT)    # 3D TAs off

    # Initialize MOT frequency switches

#    event(ch(dds,2),tResetMOT + 60*us, (100,100,0))        # TEMPORARY -- source for the 200 MHz AOM (uses RF doubler)
    print "Clear Time "+str(tClearTime)
    event(ch(dds,1), tResetMOT + 20*us, (ddsMotFrequency, 100, 0) )
    event(braggAOM1, tResetMOT + 40*us, (100, 100, 0))
   
#    event(motFrequencySwitch,  tResetMOT, 0)                       # set cooling light to 10 MHz detuned via RF switch
#    event(depumpSwitch, tResetMOT, 0) # switch off depumper

    event(repumpVariableAttenuator, tResetMOT + 10*us, 10) # set repump variable attenuator to max transmission
    event(repumpSwitch, tResetMOT + 10*us, 1)

    # Initialize Quadropole Coil
#    setQuadrupoleCurrent(tResetMOT, desiredCurrent = 0, applyCurrentRamp = False, usePrecharge = False)
#    event(sfaOutputEnableSwitch, tResetMOT, 0)
    setQuadrupoleCurrent(tResetMOT, current = 0)
    event(quadrupoleOnSwitch, tResetMOT, 1)
    event(quadrupoleChargeSwitch, tResetMOT, 0)

    # Turn off stray light sources
    event(probeLightShutter, tResetMOT, 0)         #close probe light shutter
    event(TA5, tResetMOT, 0.1)                            #turn off TA5

    # Initialize Lattice switches
    event(latticeShutter, tResetMOT, 0)
    event(ta7MOTShutter, tResetMOT, 1)
    event(ta7LatticeShutter, tResetMOT, 0)
    event(motZShutter, tResetMOT, 1)

    if(dtMOTLoad <= 0) :    # Load time must be greater than zero
        return tResetMOT

    ## Load the MOT ##  
#    setQuadrupoleCurrent(tTAOn, desiredCurrent = motQuadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 0)
    rampQuadrupoleCurrent(startTime = tTAOn, endTime = tTAOn+10*ms, startCurrent = 0, endCurrent = motQuadCoilCurrent, numberOfSteps = 10)
    
    event(quadrupoleOnSwitch, tTAOn, 1)
#    event(sfaOutputEnableSwitch, tTAOn + 100*us, 1)
    event(cooling87Shutter, tTAOn - 5*ms, 1)
    event(repumpShutter, tTAOn - 5*ms + 10*us, 1)
    turn2DMOTLightOn(tTAOn)                        #ensure that 2D light is on
    turnMOTLightOn(tTAOn)                            #TAs on

    if(leaveOn) :
        return tTAOn

    if(cMOT) :
        ## switch to a CMOT ##
#        setQuadrupoleCurrent(tTAOff - quadCoilHoldOff - dtCMOT - dtSweepToCMOT, desiredCurrent = cmotQuadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = motQuadCoilCurrent, rampRate = cmotCurrentRampRate)

        turn2DMOTLightOff(tTAOff - dtCMOT - dtSweepToCMOT - dtShutterBuffer)                          # buffer to ensure light is off before cMOT starts

        event(ch(dds,1), tTAOff - dtCMOT - dtSweepToCMOT, ((ddsMotFrequency, CMOTFrequency, dtSweepToCMOT), 100, 0) )
        
#        voltageSweep(channel = TA3, startTime = tTAOff - dtCMOT - 1.1*us-dtSweepToCMOT, sweepTime = dtSweepToCMOT, startVoltage = voltageTA3, stopVoltage = 1.6*powerReduction, numberOfEvents = 10)                 #voltageTA3=1.6
        voltageSweep(channel = TA4, startTime = tTAOff - dtCMOT-dtSweepToCMOT, sweepTime = dtSweepToCMOT, startVoltage = ta4MotVoltage, stopVoltage = (0.25+(.15*powerReduction)), numberOfEvents = 10, eventFunc = setTACurrent)    #powerReduction*ta4MotVoltage        ##ta4MotVoltage=0.4
        

        
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


def turn3DZLightOn(tTurnOn, ta4PowerScale = 1.0):
    event(TA7, tTurnOn, ta7MotVoltage)                             # z-Axis seed TA on
    setTACurrent(TA4, tTurnOn, ta4MotVoltage*ta4PowerScale)                             # z-Axis TA on                                           
    event(zAxisRfSwitch, tTurnOn, 1)
    return tTurnOn;

def turn3DZLightOnImaging(tTurnOn, ta4Voltage):
    event(TA7, tTurnOn, ta7MotVoltage)                             # z-Axis seed TA on
    setTACurrent(TA4, tTurnOn, ta4Voltage)                             # z-Axis TA on                                           
    event(zAxisRfSwitch, tTurnOn, 1)
    return tTurnOn;


def turn3DXYLightOn(tTurnOn):                         
    event(TA8, tTurnOn+10*us, voltageTA8)
    event(ta3SeedShutter, tTurnOn - 3*ms, 1)        # xy-axes on
    return tTurnOn;

def turnRFModulationOn(tTurnOn):
    event(repumpSwitch, tTurnOn, 1)                                # for faster switching, turn on repump and cooling modulation
    event(motFrequencySwitch, tTurnOn, 0)                      # turn on all cooling modulation

def turnMOTLightOn(tTurnOn):
##    event(TA2, tTurnOn - 2.2*us, voltageTA2)                   # TA on; now in 2DMOT function
##    event(TA3, tTurnOn - 1.1*us, voltageTA3)                   # TA on; now in 2DMOT function
#    event(TA7, tTurnOn, ta7MotVoltage)                             # z-Axis seed TA on
#    event(TA4, tTurnOn, ta4MotVoltage)                             # z-Axis TA on                                           
#    event(ta3SeedShutter, tTurnOn - dtShutterBuffer, 1)        # xy-axes on
#    event(zAxisRfSwitch, tTurnOn, 1)
##    event(braggAOM1, tTurnOn, braggAOM1MOT)
##    event(zAxisAom, tTurnOn - 100*us, zAxisAomMot)
    turn3DZLightOn(tTurnOn)
    turn3DXYLightOn(tTurnOn)
    turnRFModulationOn(tTurnOn)
    return tTurnOn;

def turn3DZLightOff(tTurnOff):
#    event(TA7, tTurnOff + 3.3*us, ta7OffVoltage)                    # z-Axis seed TA off
    setTACurrent(TA4, tTurnOff, ta4OffVoltage)                #z-axis TA off -- AFS increased from 0V to ta4OffVoltage
    event(zAxisRfSwitch, tTurnOff, 0)
    return tTurnOff;

def turn3DXYLightOff(tTurnOff):
    event(ta3SeedShutter, tTurnOff + 0*dtShutterBuffer-2.5*ms, 0)    #xy-axes off
    return tTurnOff;

def turnRFModulationOff(tTurnOff):
    event(repumpSwitch, tTurnOff, 0)                                # for faster switching, turn off repump and cooling modulation
    event(motFrequencySwitch, tTurnOff, 1)                      # turn off all cooling modulation

def turnMOTLightOff(tTurnOff):
##    event(TA2, tTurnOff + 2.2*us, 0)                   # TA off; now in 2DMOT function
##    event(TA3, tTurnOff + 1.1*us, 0)                   # TA off; now in 2DMOT function
#    event(TA7, tTurnOff + 3.3*us, 0)                    # z-Axis seed TA off
##    if(tTurnOff != 2100210000):
#    event(TA4, tTurnOff, ta4OffVoltage)                #z-axis TA off                                           ###COMMENTED SMD - AFS increased from 0V to ta4OffVoltage
#    event(ta3SeedShutter, tTurnOff + dtShutterBuffer, 0)    #xy-axes off
#    event(zAxisRfSwitch, tTurnOff, 0)
##    event(braggAOM1, tTurnOff, braggAOM1Off)
##    event(zAxisAom, tTurnOff + 100*us, zAxisAomOff)
    turn3DZLightOff(tTurnOff)
    turn3DXYLightOff(tTurnOff)
    turnRFModulationOff(tTurnOff)
    return tTurnOff;

def turn2DMOTLightOn(tTurnOn):
    event(TA2, tTurnOn - 2.2*us, voltageTA2)                   # 2D seed TA on
    event(TA3, tTurnOn - 1.1*us, voltageTA3)                   # 2D light TA on
    event(twoDMOTShutter, tTurnOn - dtShutterBuffer, 1)
    return tTurnOn;

def turn2DMOTLightOff(tTurnOff):
#    event(TA2, tTurnOff + 2.2*us, 0)                   # 2D seed TA off
    event(TA3, tTurnOff + 1.1*us, 0)                   # 2D light TA off
    event(twoDMOTShutter, tTurnOff, 0)
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
    if(current > 1.5):
        print 1/0
    event(channel, time, current)
