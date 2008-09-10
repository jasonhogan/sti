package edu.stanford.atom.sti.client.comm.corba;


/**
* edu/stanford/atom/sti/client/comm/corba/_ExpSequenceStub.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from ./client.idl
* Thursday, September 4, 2008 10:42:38 AM PDT
*/

public class _ExpSequenceStub extends org.omg.CORBA.portable.ObjectImpl implements edu.stanford.atom.sti.client.comm.corba.ExpSequence
{

  public String[] variables ()
  {
            org.omg.CORBA.portable.InputStream $in = null;
            try {
                org.omg.CORBA.portable.OutputStream $out = _request ("_get_variables", true);
                $in = _invoke ($out);
                String $result[] = edu.stanford.atom.sti.client.comm.corba.TStringSeqHelper.read ($in);
                return $result;
            } catch (org.omg.CORBA.portable.ApplicationException $ex) {
                $in = $ex.getInputStream ();
                String _id = $ex.getId ();
                throw new org.omg.CORBA.MARSHAL (_id);
            } catch (org.omg.CORBA.portable.RemarshalException $rm) {
                return variables (        );
            } finally {
                _releaseReply ($in);
            }
  } // variables


  //ColumnHeaders
  public edu.stanford.atom.sti.client.comm.corba.TRow[] experiments ()
  {
            org.omg.CORBA.portable.InputStream $in = null;
            try {
                org.omg.CORBA.portable.OutputStream $out = _request ("_get_experiments", true);
                $in = _invoke ($out);
                edu.stanford.atom.sti.client.comm.corba.TRow $result[] = edu.stanford.atom.sti.client.comm.corba.TRowSeqHelper.read ($in);
                return $result;
            } catch (org.omg.CORBA.portable.ApplicationException $ex) {
                $in = $ex.getInputStream ();
                String _id = $ex.getId ();
                throw new org.omg.CORBA.MARSHAL (_id);
            } catch (org.omg.CORBA.portable.RemarshalException $rm) {
                return experiments (        );
            } finally {
                _releaseReply ($in);
            }
  } // experiments


  //All return true on error
  public boolean appendRow (String[] newRow)
  {
            org.omg.CORBA.portable.InputStream $in = null;
            try {
                org.omg.CORBA.portable.OutputStream $out = _request ("appendRow", true);
                edu.stanford.atom.sti.client.comm.corba.TStringSeqHelper.write ($out, newRow);
                $in = _invoke ($out);
                boolean $result = $in.read_boolean ();
                return $result;
            } catch (org.omg.CORBA.portable.ApplicationException $ex) {
                $in = $ex.getInputStream ();
                String _id = $ex.getId ();
                throw new org.omg.CORBA.MARSHAL (_id);
            } catch (org.omg.CORBA.portable.RemarshalException $rm) {
                return appendRow (newRow        );
            } finally {
                _releaseReply ($in);
            }
  } // appendRow

  public boolean moveRow (int oldPos, int newPos)
  {
            org.omg.CORBA.portable.InputStream $in = null;
            try {
                org.omg.CORBA.portable.OutputStream $out = _request ("moveRow", true);
                $out.write_ulong (oldPos);
                $out.write_ulong (newPos);
                $in = _invoke ($out);
                boolean $result = $in.read_boolean ();
                return $result;
            } catch (org.omg.CORBA.portable.ApplicationException $ex) {
                $in = $ex.getInputStream ();
                String _id = $ex.getId ();
                throw new org.omg.CORBA.MARSHAL (_id);
            } catch (org.omg.CORBA.portable.RemarshalException $rm) {
                return moveRow (oldPos, newPos        );
            } finally {
                _releaseReply ($in);
            }
  } // moveRow

  public boolean editRow (int pos, String[] newRow)
  {
            org.omg.CORBA.portable.InputStream $in = null;
            try {
                org.omg.CORBA.portable.OutputStream $out = _request ("editRow", true);
                $out.write_ulong (pos);
                edu.stanford.atom.sti.client.comm.corba.TStringSeqHelper.write ($out, newRow);
                $in = _invoke ($out);
                boolean $result = $in.read_boolean ();
                return $result;
            } catch (org.omg.CORBA.portable.ApplicationException $ex) {
                $in = $ex.getInputStream ();
                String _id = $ex.getId ();
                throw new org.omg.CORBA.MARSHAL (_id);
            } catch (org.omg.CORBA.portable.RemarshalException $rm) {
                return editRow (pos, newRow        );
            } finally {
                _releaseReply ($in);
            }
  } // editRow

  public void editDone (int pos, boolean newDone)
  {
            org.omg.CORBA.portable.InputStream $in = null;
            try {
                org.omg.CORBA.portable.OutputStream $out = _request ("editDone", true);
                $out.write_ulong (pos);
                $out.write_boolean (newDone);
                $in = _invoke ($out);
                return;
            } catch (org.omg.CORBA.portable.ApplicationException $ex) {
                $in = $ex.getInputStream ();
                String _id = $ex.getId ();
                throw new org.omg.CORBA.MARSHAL (_id);
            } catch (org.omg.CORBA.portable.RemarshalException $rm) {
                editDone (pos, newDone        );
            } finally {
                _releaseReply ($in);
            }
  } // editDone

  // Type-specific CORBA::Object operations
  private static String[] __ids = {
    "IDL:STI_Client_Server/ExpSequence:1.0"};

  public String[] _ids ()
  {
    return (String[])__ids.clone ();
  }

  private void readObject (java.io.ObjectInputStream s) throws java.io.IOException
  {
     String str = s.readUTF ();
     String[] args = null;
     java.util.Properties props = null;
     org.omg.CORBA.Object obj = org.omg.CORBA.ORB.init (args, props).string_to_object (str);
     org.omg.CORBA.portable.Delegate delegate = ((org.omg.CORBA.portable.ObjectImpl) obj)._get_delegate ();
     _set_delegate (delegate);
  }

  private void writeObject (java.io.ObjectOutputStream s) throws java.io.IOException
  {
     String[] args = null;
     java.util.Properties props = null;
     String str = org.omg.CORBA.ORB.init (args, props).object_to_string (this);
     s.writeUTF (str);
  }
} // class _ExpSequenceStub
