package edu.stanford.atom.sti.client.comm.corba;


/**
* edu/stanford/atom/sti/client/comm/corba/TValMixedHelper.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from ./client.idl
* Thursday, September 4, 2008 10:42:38 AM PDT
*/

abstract public class TValMixedHelper
{
  private static String  _id = "IDL:STI_Client_Server/TValMixed/TValMixed:1.0";

  public static void insert (org.omg.CORBA.Any a, edu.stanford.atom.sti.client.comm.corba.TValMixed that)
  {
    org.omg.CORBA.portable.OutputStream out = a.create_output_stream ();
    a.type (type ());
    write (out, that);
    a.read_value (out.create_input_stream (), type ());
  }

  public static edu.stanford.atom.sti.client.comm.corba.TValMixed extract (org.omg.CORBA.Any a)
  {
    return read (a.create_input_stream ());
  }

  private static org.omg.CORBA.TypeCode __typeCode = null;
  synchronized public static org.omg.CORBA.TypeCode type ()
  {
    if (__typeCode == null)
    {
      org.omg.CORBA.TypeCode _disTypeCode0;
      _disTypeCode0 = edu.stanford.atom.sti.device.comm.corba.TValueHelper.type ();
      org.omg.CORBA.UnionMember[] _members0 = new org.omg.CORBA.UnionMember [4];
      org.omg.CORBA.TypeCode _tcOf_members0;
      org.omg.CORBA.Any _anyOf_members0;

      // Branch for number (case label ValueNumber)
      _anyOf_members0 = org.omg.CORBA.ORB.init ().create_any ();
      edu.stanford.atom.sti.device.comm.corba.TValueHelper.insert (_anyOf_members0, edu.stanford.atom.sti.device.comm.corba.TValue.ValueNumber);
      _tcOf_members0 = org.omg.CORBA.ORB.init ().get_primitive_tc (org.omg.CORBA.TCKind.tk_double);
      _members0[0] = new org.omg.CORBA.UnionMember (
        "number",
        _anyOf_members0,
        _tcOf_members0,
        null);

      // Branch for stringVal (case label ValueString)
      _anyOf_members0 = org.omg.CORBA.ORB.init ().create_any ();
      edu.stanford.atom.sti.device.comm.corba.TValueHelper.insert (_anyOf_members0, edu.stanford.atom.sti.device.comm.corba.TValue.ValueString);
      _tcOf_members0 = org.omg.CORBA.ORB.init ().create_string_tc (0);
      _members0[1] = new org.omg.CORBA.UnionMember (
        "stringVal",
        _anyOf_members0,
        _tcOf_members0,
        null);

      // Branch for triplet (case label ValueDDSTriplet)
      _anyOf_members0 = org.omg.CORBA.ORB.init ().create_any ();
      edu.stanford.atom.sti.device.comm.corba.TValueHelper.insert (_anyOf_members0, edu.stanford.atom.sti.device.comm.corba.TValue.ValueDDSTriplet);
      _tcOf_members0 = edu.stanford.atom.sti.client.comm.corba.TDDSHelper.type ();
      _members0[2] = new org.omg.CORBA.UnionMember (
        "triplet",
        _anyOf_members0,
        _tcOf_members0,
        null);

      // Branch for meas (case label ValueMeas)
      _anyOf_members0 = org.omg.CORBA.ORB.init ().create_any ();
      edu.stanford.atom.sti.device.comm.corba.TValueHelper.insert (_anyOf_members0, edu.stanford.atom.sti.device.comm.corba.TValue.ValueMeas);
      _tcOf_members0 = org.omg.CORBA.ORB.init ().get_primitive_tc (org.omg.CORBA.TCKind.tk_boolean);
      _members0[3] = new org.omg.CORBA.UnionMember (
        "meas",
        _anyOf_members0,
        _tcOf_members0,
        null);
      __typeCode = org.omg.CORBA.ORB.init ().create_union_tc (edu.stanford.atom.sti.client.comm.corba.TValMixedHelper.id (), "TValMixed", _disTypeCode0, _members0);
    }
    return __typeCode;
  }

  public static String id ()
  {
    return _id;
  }

  public static edu.stanford.atom.sti.client.comm.corba.TValMixed read (org.omg.CORBA.portable.InputStream istream)
  {
    edu.stanford.atom.sti.client.comm.corba.TValMixed value = new edu.stanford.atom.sti.client.comm.corba.TValMixed ();
    edu.stanford.atom.sti.device.comm.corba.TValue _dis0 = null;
    _dis0 = edu.stanford.atom.sti.device.comm.corba.TValueHelper.read (istream);
    switch (_dis0.value ())
    {
      case edu.stanford.atom.sti.device.comm.corba.TValue._ValueNumber:
        double _number = (double)0;
        _number = istream.read_double ();
        value.number (_number);
        break;
      case edu.stanford.atom.sti.device.comm.corba.TValue._ValueString:
        String _stringVal = null;
        _stringVal = istream.read_string ();
        value.stringVal (_stringVal);
        break;
      case edu.stanford.atom.sti.device.comm.corba.TValue._ValueDDSTriplet:
        edu.stanford.atom.sti.client.comm.corba.TDDS _triplet = null;
        _triplet = edu.stanford.atom.sti.client.comm.corba.TDDSHelper.read (istream);
        value.triplet (_triplet);
        break;
      case edu.stanford.atom.sti.device.comm.corba.TValue._ValueMeas:
        boolean _meas = false;
        _meas = istream.read_boolean ();
        value.meas (_meas);
        break;
    }
    return value;
  }

  public static void write (org.omg.CORBA.portable.OutputStream ostream, edu.stanford.atom.sti.client.comm.corba.TValMixed value)
  {
    edu.stanford.atom.sti.device.comm.corba.TValueHelper.write (ostream, value.discriminator ());
    switch (value.discriminator ().value ())
    {
      case edu.stanford.atom.sti.device.comm.corba.TValue._ValueNumber:
        ostream.write_double (value.number ());
        break;
      case edu.stanford.atom.sti.device.comm.corba.TValue._ValueString:
        ostream.write_string (value.stringVal ());
        break;
      case edu.stanford.atom.sti.device.comm.corba.TValue._ValueDDSTriplet:
        edu.stanford.atom.sti.client.comm.corba.TDDSHelper.write (ostream, value.triplet ());
        break;
      case edu.stanford.atom.sti.device.comm.corba.TValue._ValueMeas:
        ostream.write_boolean (value.meas ());
        break;
    }
  }

}
