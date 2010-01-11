cd .\..\..\idl\


idlj -fall -emitAll -pkgTranslate STI edu.stanford.atom.sti.corba -pkgTranslate Client_Server edu.stanford.atom.sti.corba.Client_Server -pkgTranslate Types edu.stanford.atom.sti.corba.Types -td .\..\client\src .\orbTypes.idl

idlj -fall -emitAll -pkgTranslate STI edu.stanford.atom.sti.corba -pkgTranslate Client_Server edu.stanford.atom.sti.corba.Client_Server -pkgTranslate Types edu.stanford.atom.sti.corba.Types -td .\..\client\src .\client.idl

idlj -emitAll -pkgTranslate Remote_File_Server edu.stanford.atom.sti.RemoteFileServer.comm.corba -td .\..\client\src .\FileServer.idl


pause