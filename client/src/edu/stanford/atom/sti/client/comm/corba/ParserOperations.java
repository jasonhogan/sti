package edu.stanford.atom.sti.client.comm.corba;


/**
* edu/stanford/atom/sti/client/comm/corba/ParserOperations.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from ./client.idl
* Thursday, September 4, 2008 10:42:38 AM PDT
*/

public interface ParserOperations 
{

  //Set this before parsing
  edu.stanford.atom.sti.client.comm.corba.TOverwritten[] overwritten ();

  //Set this before parsing
  void overwritten (edu.stanford.atom.sti.client.comm.corba.TOverwritten[] newOverwritten);
  boolean lockOnParse ();
  void lockOnParse (boolean newLockOnParse);

  //Both return true on error
  boolean parseFile (String filename);
  boolean parseString (String code);

  //Harvest the parsing result here:
  String outMsg ();
  String errMsg ();
  String mainFile ();
  edu.stanford.atom.sti.client.comm.corba.TChannel[] channels ();
  String[] files ();
  edu.stanford.atom.sti.client.comm.corba.TVariable[] variables ();
  edu.stanford.atom.sti.client.comm.corba.TEvent[] events ();
} // interface ParserOperations
