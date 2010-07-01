#setvar('TA2voltageSeq',1.4)
#setvar('TA2voltage', 1.4)
#
#event(TA3, ramseyStartTime, 0)             # TA3 off during depump/trap
#event(TA2, ramseyStartTime, TA2voltage)             # Turn TA2 down
#event(repumpFrequencySwitchX, ramseyStartTime, 1)     #setting repump to 3223 * 2 MHz (taking the 2nd order sideband)
#event(motFrequencySwitch,ramseyStartTime, 1)     # setting cooling light to Rb85 cool freq (should be no 85 atoms about)
#event(repumpFrequencySwitchX, ramseyStartTime + dtRabiPulse, 0)    # set repump back to 2568 MHz * 2
#event(motFrequencySwitch, ramseyStartTime + dtRabiPulse, 0)    # set cooling back to 1076 MHz
#event(TA2, ramseyStartTime + dtRabiPulse, 0)             # Turn TA2 off