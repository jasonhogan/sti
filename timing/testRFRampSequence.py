s = 1000000000.0
ms = 1000000.0

variables.append('vca3')
variables.append('dtRamp3')

vca3Lower = 5.0
vca3Upper = 7.5
vca3Range = vca3Upper - vca3Lower
deltavca3 = .5

dtRamp3Lower = 0.5*s
dtRamp3Upper = 2.5*s
dtRamp3Range = dtRamp3Upper - dtRamp3Lower
deltadtRamp3 = .5*s


for dV in range(0,vca3Range/deltavca3+1) :
    for dt in range(0, dtRamp3Range/deltadtRamp3+1) :
        experiments.append([vca3Lower+dV*deltavca3, dtRamp3Lower+dt*deltadtRamp3])
