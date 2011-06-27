include('microwaveKnifeFunction.py')
include('approximateExponentialSweep.py')
from math import pow

### VCAs ######################################################################
## VCA's control RF knife ramps
## ~4.5 is off. Frequencies are in MHz.
## Power should drop and ramps should get faster as you evaporate. 
## Several ramps are defined here, but not all may be executed. See evaporativeCoolingFunction
setvar('vca1', 10)    #6.5

setvar('f0', 120) #120
setvar('f1', 3*(20 - 0))       #3*4

setvar('dtRamp1', 6.75*s) #5.75
#setvar('dtRamp1', ( (f0-f1)*12.0*s/(120-12)) )

setvar('vca2', 10) 
setvar('f2', 3*(10))

sweepRate=(3*4 - 3*(0.01))/(1*s)
#setvar('dtRamp2', (f1-f2)/sweepRate )  #1.5*s
setvar('dtRamp2', 2.5*s )  #1.5*s

setvar('vca3', 10) #5.75
setvar('f3', 3*4)
#setvar('dtRamp3', (f2-f3)/sweepRate ) #0.6*s
setvar('dtRamp3', 4*s ) #0.6*s

setvar('vca4', 10) #5.75
setvar('f4', 3*2) #3*2
setvar('dtRamp4', 1.5*s ) #0.6*s


setvar('vca5', 10) #5.75
setvar('f5', 3*.2)
setvar('dtRamp5', 4*s ) #0.6*s

setvar('dtHoldCut', 0*s)


###############################################################################

def evaporate(tStart, rampNumber = 1):

    if (rampNumber < 1):
        return tStart

    rampList=[]
    
    #################### events #######################
    internalTime = tStart

    ### Turn microwave horn on during evaporation ######
    event(sixPointEightGHzSwitch, internalTime, 1)

    ddsRbfreq = ddsRbResonanceFreq

    evapTime = internalTime
    tEnd = evapTime
    tFrequencyRampStart=internalTime
        
    ###  Ramp 1 ###########################################
    if (rampNumber >= 1) :
        event(rfKnifeAmplitude,evapTime +  10*us, vca1)
        rampList.append((ddsRbfreq - f0, ddsRbfreq - f1, dtRamp1/s))
        tEnd = evapTime + dtRamp1
###        depumpMOT(evapTime + (dtRamp1/2), pumpingTime = 200*us)
    #######################################################

#    event(rfKnifeAmplitude, evapTime +  3*s, 3)



   ### Ramp 2 ############################################
    if (rampNumber >= 2) :
        evapTime = tEnd + 10*ms
        event(rfKnifeAmplitude, evapTime - 4*ms, vca2)
        rampList.append((ddsRbfreq - f1, ddsRbfreq - f2, dtRamp2/s))
#        rampList.append((ddsRbfreq - f2, ddsRbfreq - f2, dtRamp2/s))

        #Add a depump during the evaporation
        depumpMOT(tEnd, pumpingTime = 200*us)

        tEnd = evapTime + dtRamp2
    #######################################################

    
#    tEnd = max(tEnd, tRampFinished)    


    ### Ramp 3 ############################################
    if (rampNumber >= 3):
#        evapTime = tEnd + 20*ms
#        event(rfKnifeAmplitude, evapTime - 10*ms, vca3)
        evapTime = tEnd + 1.4*ms
        event(rfKnifeAmplitude, evapTime - 5*us, vca3)
        rampList.append((ddsRbfreq - f2, ddsRbfreq - f3, dtRamp3/s))
        tEnd = evapTime + dtRamp3
    #######################################################


    ### Ramp 4 ############################################
    if (rampNumber >= 4):
        evapTime = tEnd + 1.4*ms
        event(rfKnifeAmplitude, evapTime - 5*us, vca4)
        rampList.append((ddsRbfreq - f3, ddsRbfreq - f4, dtRamp4/s))
#        rampList.append((ddsRbfreq - f4, ddsRbfreq - f4, dtRamp4/s))
        tEnd = evapTime + dtRamp4
    #######################################################



#    tRampDown=tEnd+ 0*0.1*ms
#
#    tRampFinished = rampDownQuadCoils(tRampDown+1*ms, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = 100, rapidOff = False, quadRampRate = 1.0)
#
#    tEnd = tRampFinished


    ### Ramp 5 ############################################
    if (rampNumber >= 5):
        evapTime = tEnd + 1.4*ms
        event(rfKnifeAmplitude, evapTime - 5*us, vca5)
        rampList.append((ddsRbfreq - f4, ddsRbfreq - f5, dtRamp5/s))
        tEnd = evapTime + dtRamp5
    #######################################################


    ## Run the DDS sweep for all linear segments
    event(ddsRfKnife, tFrequencyRampStart, rampList)

    tEnd += dtHoldCut

    event(rfKnifeAmplitude, tEnd + 0*0.1*ms, 0)
    #### Turn off microwave horn at end of evaporation)
    event(sixPointEightGHzSwitch, tEnd + 0*0.1*ms, 0)
   

#    tRampDown=tEnd
#    tRampFinished = rampDownQuadCoils(tRampDown+1*ms, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = 100, rapidOff = False, quadRampRate = 1.0)
#    tEnd = tRampFinished
#    tEnd += 10*ms    #For imagining

    return tEnd
