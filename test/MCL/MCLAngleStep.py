from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

urad = 0.000001

# Set description used by program

nanoDrive=dev('MCL NanoDrive','ep-timing1.stanford.edu',0)
mirrorPosition=ch(nanoDrive,0)
measMirrorPosition=ch(nanoDrive,1)

# defined variables
setvar('thetaStart', 150*urad)
setvar('phiStart',3.14159/3)
setvar('zStart',15)
setvar('dt', 10*ms)
setvar('startAngle',0)
setvar('stepAngle',0*urad)
setvar('stepZ',15)
setvar('endAngle',0)
setvar('dtSetAxes',300*ms)
# Global definitions

t0 = 11*us + 2*ms

event(mirrorPosition, t0, (thetaStart,phiStart,zStart))
#event(mirrorPosition, t0+dtSetAxes, (stepAngle,phiStart,stepZ))
#event(mirrorPosition, t0+2*dtSetAxes, (thetaStart,phiStart,stepZ))
#event(mirrorPosition, t0+3*dtSetAxes, (-stepAngle,phiStart,stepZ))
#event(mirrorPosition, t0+4*dtSetAxes, (thetaStart,phiStart,stepZ))

#for i in range(1, 30):
#    meas(measMirrorPosition, t0+dtSetAxes+i*dt)

#set back to zero with a certain buffer
#event(mirrorPosition, t0+(3)*dtSetAxes, (thetaStart,phiStart,zStart))
#event(mirrorPosition, t0+(3)*dtSetAxes+dt, (thetaStart,phiStart,zStart))

#event(mirrorPosition, t0+(3)*dtSetAxes+dt, (thetaStart,phiStart,stepZ))
