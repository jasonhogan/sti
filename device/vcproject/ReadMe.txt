========================================================================
       STATIC LIBRARY : deviceLib
========================================================================


Project for building the STI_Device library.  STI_Device is the base 
class used for all network devices. The resulting library file is 
STI_Device.lib.  Programs with subclasses of STI_Device must static link
with this library. 

Jason Hogan (hogan@stanford.edu)



========================================================================
       CONSOLE APPLICATION : testDevice
========================================================================


Project for building an example STI_Device subcalss called 'testDevice.'
This device doesn't do anything useful except for demonstrating network
communication with the STI server.

Jason Hogan (hogan@stanford.edu)
