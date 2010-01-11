from timing import *

do1 = dev('DigOutx24 v1', '192.168.0.12', 1)
ao1 = dev('AnOutx2 v1',   '192.168.0.12', 2)
cam = dev('AndorCam v1',  '192.168.0.13', 1)
setvar('MOT_shutter',     ch(do1, 1)) # The shutter in the MOT beam
setvar('MOTcoil_MOSFET',  ch(do1, 2)) # MOSFET for the MOT coil
setvar('MOTcoil_current', ch(ao1, 1)) # Analog current control
setvar('MOT_curr_callib', 1.034)      # Callib. for MOTcoil_current in bin/V
setvar('Camera_Trigger',  ch(do1, 3)) # Camera trigger channel
setvar('Camera',          ch(cam, 1)) # Camera picture channel
