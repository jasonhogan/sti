This aims to be a generic device, configurable via an XML file.

Format of XML file is not yet hammered out; talk to Brannon (please give suggestions!).

Sample XML and more info should be available on the Trac wiki.

Invocation (on Windows) should be something like:
.\genericDevice.exe --xml DeviceFile.xml --ini ConfigFile.ini -- -ORBendPointPublish giop:tcp:%ipaddress%: -ORBInitRef NameService=corbaname::%ipaddress%:2809 %GLOBAL_OPTS%