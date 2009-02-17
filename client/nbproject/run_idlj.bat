cd .\..\..\idl\

idlj -fall -emitAll -pkgTranslate STI_Client_Server edu.stanford.atom.sti.client.comm.corba -pkgTranslate STI_Server_Device edu.stanford.atom.sti.device.comm.corba -td .\..\client\src .\client.idl

idlj -emitAll -pkgTranslate Remote_File_Server edu.stanford.atom.sti.RemoteFileServer.comm.corba -td .\..\client\src .\FileServer.idl


pause