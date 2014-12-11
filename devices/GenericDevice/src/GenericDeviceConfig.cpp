/*! \file XMLDuder.cpp 
 *  \author Brannon Klopfer
 *  \author http://www.yolinux.com/TUTORIALS/XML-Xerces-C.html
 *  \brief Source-file for the class GenericDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Brannon Klopfer <bbklopfer@stanford.edu>
 *
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

// I think we should go with Jason's XML framework and simplify this whole thing.
// TODO: Error handling, cleanup, etc.

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "GenericDeviceConfig.hpp"
#include "../../gpibController/src/gpibControllerDevice.h"
#include <vector>

#include <boost/algorithm/string.hpp> // for trim()
#include <boost/lexical_cast.hpp> // for casting strings to numerical

using namespace xercesc;
using namespace std;

/**
 *  Constructor initializes xerces-C libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The xerces-C DOM parser infrastructure is initialized.
 */

GenericDeviceConfig::GenericDeviceConfig()
{
	try
	{
		XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
	}
	catch( XMLException& e )
	{
		char* message = XMLString::transcode( e.getMessage() );
		cerr << "XML toolkit initialization error: " << message << endl;
		XMLString::release( &message );
		// throw exception here to return ERROR_XERCES_INIT
	}

	// Tags and attributes used in XML file.
	// Can't call transcode till after Xerces Initialize()
	TAG_device  = XMLString::transcode(DEVICE_STR);
	TAG_channel = XMLString::transcode(CHANNEL_STR);
	TAG_channels = XMLString::transcode(CHANNELS_STR);
	TAG_cmd = XMLString::transcode(CMD_STR);
	TAG_getcmd = XMLString::transcode(GETCMD_STR);
	TAG_setcmd = XMLString::transcode(SETCMD_STR);
	TAG_routine = XMLString::transcode(ROUTINE_STR);
	TAG_init = XMLString::transcode(INIT_STR);
	TAG_attr = XMLString::transcode(ATTR_STR);
	TAG_attrs = XMLString::transcode(ATTRS_STR);
	TAG_value = XMLString::transcode(VALUE_STR);
	TAG_partner = XMLString::transcode(PARTNER_STR);
	TAG_partners = XMLString::transcode(PARTNERS_STR);

	ATTR_proto = XMLString::transcode(PROTO_STR);
	ATTR_name = XMLString::transcode(NAME_STR);
	ATTR_prettyname = XMLString::transcode(PRETTYNAME_STR);
	ATTR_output = XMLString::transcode(OUTPUT_STR);
	ATTR_nargs = XMLString::transcode(NARGS_STR);
	ATTR_equiv = XMLString::transcode("equiv");
	ATTR_echo = XMLString::transcode("echo");
	ATTR_charDelay = XMLString::transcode("chardelay");
	ATTR_querySleep = XMLString::transcode("querySleep");
	ATTR_chan = XMLString::transcode(CHANNEL_STR);
	ATTR_num = XMLString::transcode(NUM_STR);
	ATTR_module = XMLString::transcode("module");
	ATTR_addr = XMLString::transcode("addr");
	ATTR_readtype = XMLString::transcode("readtype");

	deviceType = TYPE_UNKNOWN;

	channels = new std::vector<Channel>();
	devAttributes = new std::map<std::string, DevAttribute>();
	serialSettings = NULL;
	gpibSettings = NULL;

	m_ConfigFileParser = new XercesDOMParser;
	configFile = NULL;

	partnerDevices = new vector<ConfigPartnerDevice>();
}

// TODO: MAKE SURE WE CLEAN UP AFTER OURSELVES! ATTRS/XML* STUFF!!!
/**
 *  Class destructor frees memory used to hold the XML tag and 
 *  attribute definitions. It als terminates use of the xerces-C
 *  framework.
 */
GenericDeviceConfig::~GenericDeviceConfig()
{
   // Free memory
	delete partnerDevices;
	delete m_ConfigFileParser;
	m_ConfigFileParser = NULL;
	delete channels;
	channels = NULL;
	delete devAttributes;
	devAttributes = NULL;
	delete serialSettings;
	serialSettings = NULL;
	delete gpibSettings;
	gpibSettings = NULL;

	if(m_ATTR_name)   XMLString::release( &m_ATTR_name );
	if(m_ATTR_prettyname)   XMLString::release( &m_ATTR_prettyname );

	try
	{
		XMLString::release(&TAG_device);
		XMLString::release(&TAG_channel);
		XMLString::release(&TAG_channels);
		XMLString::release(&TAG_cmd);
		XMLString::release(&TAG_getcmd);
		XMLString::release(&TAG_setcmd);
		XMLString::release(&TAG_routine);
		XMLString::release(&TAG_init);
		XMLString::release(&TAG_attr);
		XMLString::release(&TAG_attrs);
		XMLString::release(&TAG_value);
		XMLString::release(&TAG_partners);
		XMLString::release(&TAG_partner);

		XMLString::release(&ATTR_name);
		XMLString::release(&ATTR_prettyname);
		XMLString::release(&ATTR_nargs);
		XMLString::release(&ATTR_output);
		XMLString::release(&ATTR_equiv);
		XMLString::release(&ATTR_proto);
		XMLString::release(&ATTR_echo);
		XMLString::release(&ATTR_charDelay);
		XMLString::release(&ATTR_chan);
		XMLString::release(&ATTR_num);
		XMLString::release(&ATTR_module);
		XMLString::release(&ATTR_addr);
		XMLString::release(&ATTR_querySleep);
		XMLString::release(&ATTR_readtype);
	}
	catch( ... )
	{
		cerr << "Unknown exception encountered in TagNamesdtor" << endl;
	}

	// Terminate Xerces
	try
	{
		XMLPlatformUtils::Terminate();  // Terminate after release of memory
	}
	catch( xercesc::XMLException& e )
	{
		char* message = xercesc::XMLString::transcode( e.getMessage() );

		cerr << "XML ttolkit teardown error: " << message << endl;
		XMLString::release( &message );
	}
}

// TODO: Clean all this up, buckets of code repitition here...
void GenericDeviceConfig::processChannels(DOMElement* xchannels)
{
	DOMNodeList*      children = xchannels->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	vector<Command> cmds;

	for(XMLSize_t i = 0; i < nodeCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
			currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement
						= dynamic_cast< xercesc::DOMElement* >( currentNode );	
			if(XMLString::equals(currentElement->getTagName(), TAG_channel)) {
				processChannel(currentElement);
			}
		}
	}
}

void GenericDeviceConfig::processPartners(DOMElement* xpartners)
{
	DOMNodeList* children = xpartners->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();

	for(XMLSize_t i = 0; i < nodeCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
			currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement
				= dynamic_cast< xercesc::DOMElement* >( currentNode );
			if(XMLString::equals(currentElement->getTagName(), TAG_partner)) {
				processPartner(currentElement);
			}
		}
	}
}

void GenericDeviceConfig::processPartner(DOMElement* xpartner)
{
	ConfigPartnerDevice pd;
	pd.name = XMLString::transcode(xpartner->getAttribute(ATTR_name));
	pd.addr = XMLString::transcode(xpartner->getAttribute(ATTR_addr));
	istringstream(XMLString::transcode(xpartner->getAttribute(ATTR_module)))
		>> pd.module;

	DOMNodeList* children = xpartner->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();

	for(XMLSize_t i = 0; i < nodeCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&
			currentNode->getNodeType() == DOMNode::ELEMENT_NODE )
		{
			DOMElement* currentElement
			            = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if(XMLString::equals(currentElement->getTagName(), TAG_channel)) {
				unsigned short chan;
				istringstream(XMLString::transcode(
				              currentElement->getAttribute(ATTR_num)))
				              >> chan;
				pd.channels.push_back(chan);
			}
		}
	}

	partnerDevices->push_back(pd);
}

void GenericDeviceConfig::processAttributes(DOMElement* xattributes)
{
	DOMNodeList*      children = xattributes->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	vector<Command> cmds;

	for (XMLSize_t i = 0; i < nodeCount; i++) {
		DOMNode* currentNode = children->item(i);
		if (currentNode->getNodeType() &&  // true is not NULL
			currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement
						= dynamic_cast< xercesc::DOMElement* >( currentNode );	
			if (XMLString::equals(currentElement->getTagName(), TAG_attr)) {
				processAttribute(currentElement);
			}
		}
	}
}

void GenericDeviceConfig::processChannel(DOMElement* xchannel)
{
	const XMLCh* xmlch_ATTR_name
			= xchannel->getAttribute(ATTR_name);
	m_ATTR_name = XMLString::transcode(xmlch_ATTR_name);

	const XMLCh* xmlch_ATTR_prettyname
			= xchannel->getAttribute(ATTR_prettyname);
	m_ATTR_prettyname = XMLString::transcode(xmlch_ATTR_prettyname);

	DOMNodeList*      children = xchannel->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	//child->getParentNode()->getNodeName
	Channel channel;
	channel.name = XMLString::transcode(xchannel->getAttribute(ATTR_name));
	channel.prettyname = XMLString::transcode(xchannel->getAttribute(ATTR_prettyname));

	for(XMLSize_t i = 0; i < nodeCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
			currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement
						= dynamic_cast< xercesc::DOMElement* >( currentNode );	
			if(XMLString::equals(currentElement->getTagName(), TAG_routine)) {
				// TODO: Make pretty! Refactor! Recursion?
				// process a routine
				int nargs = 0;
				if (currentElement->hasAttribute(ATTR_nargs))
					istringstream(XMLString::transcode(currentElement->getAttribute(ATTR_nargs))) >> nargs;
				channel.routines.insert(std::pair<int, vector<Command>> (nargs, processRoutine(currentElement)));
			}
		}
	}

	channels->push_back(channel);
}

vector<Command> GenericDeviceConfig::processRoutine(DOMElement* routine)
{
	DOMNodeList*      children = routine->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	vector<Command> cmds;

	for(XMLSize_t i = 0; i < nodeCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
			currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement
						= dynamic_cast< xercesc::DOMElement* >( currentNode );	
			if(XMLString::equals(currentElement->getTagName(), TAG_cmd)) {
				// TODO: Make pretty! Refactor! Recursion?
				// process a routine
				cmds.push_back(processCommand(currentElement));
			}
		}
	}
	
	return cmds;
}

Command GenericDeviceConfig::processCommand(DOMElement* currentElement)
{
	Command cmd;
	cmd.cmdstr = XMLString::transcode(currentElement->getTextContent());
	istringstream(XMLString::transcode(currentElement->getAttribute(ATTR_output))) >> cmd.output;

	return cmd;
}

void GenericDeviceConfig::setIniConfig(ConfigFile *cf)
{
	configFile = cf;
}

ConfigFile* GenericDeviceConfig::getIniConfig()
{
	return configFile;
}

void GenericDeviceConfig::getAttributeValueFromDisplayString(std::string name, std::string displaystr, std::string* val)
{
	DevAttribute da = devAttributes->at(name);
	if (!da.dispToCmd[displaystr].empty()) {
		*val = da.dispToCmd[displaystr];
	} else {
		*val = displaystr; // In case displayed and internal are the same.
	}
}

// Not sure if this is actually useful, given getAttributeLabel()
void GenericDeviceConfig::getAttributeDisplayStringFromValue(std::string name, std::string val, std::string* displaystr)
{
	DevAttribute da = devAttributes->at(name);
	if (!da.equivValues[val].empty()) {
		*displaystr = da.equivValues[val];
	} else {
		*displaystr = val; // In case displayed and internal are the same.
	}
}

//  Trying to match via myriad methods...I think sticking to user-defined maps is nicer, but this is good for edge cases involving numbers...
bool GenericDeviceConfig::getAttributeLabel(std::string name, std::string value, std::string* newLabel)
{
	DevAttribute da = devAttributes->at(name);

	// If no values were listed, assume that we were passed a valid value:
	if (da.values.empty()) {
		*newLabel = value;

		return true;
	}

	// String match:
	if (std::find(da.values.begin(), da.values.end(), value) != da.values.end()) {
		*newLabel = value;

		return true;
	}

	// Equiv match:
	if (da.equivValues.count(value)) {
		*newLabel = da.equivValues.at(value);

		return true;
	}

	// Match numeric value, without regards to formatting:
	double valueNum, newValueNum;
	bool valueIsNum = true;
	try {
		valueNum = boost::lexical_cast<double>(value);
		for (int i = 0; i < da.values.size(); i++) {
			try {
				newValueNum = boost::lexical_cast<double>(da.values.at(i));
				if (valueNum == newValueNum) {
					*newLabel = da.values.at(i);
							
					return true;
				}
			} catch (const boost::bad_lexical_cast &exc) { } // FIXME: Supress warnings about unused exc...
		}
	} catch ( const boost::bad_lexical_cast &exc ) { }

	// Default is to just set to first value.
	// This is mildly dangerous in that it could give a false sense of security...
	*newLabel = da.values.at(0);
	cerr << "Cannot find proper label! Name, value: '"
		<< name << "', '" << value << "'" << endl
		<< "Defaulting to first value, " << *newLabel << "'" << endl;

	return false;
}

void GenericDeviceConfig::processAttribute(DOMElement* xattr)
{
	DOMNodeList*      children = xattr->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	DevAttribute attr;
	attr.name = XMLString::transcode(xattr->getAttribute(ATTR_name));
	attr.prettyname = XMLString::transcode(xattr->getAttribute(ATTR_prettyname));

	for (XMLSize_t i = 0; i < nodeCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if (currentNode->getNodeType() &&  // true is not NULL
			currentNode->getNodeType() == DOMNode::ELEMENT_NODE) // is element 
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement
						= dynamic_cast< xercesc::DOMElement* >( currentNode );
			if (XMLString::equals(currentElement->getTagName(), TAG_getcmd)) {
				attr.getCommand = processCommand(currentElement);
			} else if (XMLString::equals(currentElement->getTagName(), TAG_setcmd)) {
				attr.setCommand = processCommand(currentElement);
			}  else if (XMLString::equals(currentElement->getTagName(), TAG_cmd)) {
				// If we have a <command /> guy, then do the obvious get/set of CMD?/CMD <VALUE>
				attr.setCommand = processCommand(currentElement);
				attr.setCommand.cmdstr = attr.setCommand.cmdstr + " $1";
				attr.setCommand.output = false;
				attr.getCommand = processCommand(currentElement);
				attr.getCommand.cmdstr = attr.getCommand.cmdstr + "?";
				attr.getCommand.output = true;
			} else if(XMLString::equals(currentElement->getTagName(), TAG_value)) {
				attr.values.push_back(XMLString::transcode(currentElement->getTextContent()));
				string equiv;
				if (currentElement->hasAttribute(ATTR_equiv)) {
					equiv = XMLString::transcode(currentElement->getAttribute(ATTR_equiv));
				} else {
					equiv = XMLString::transcode(currentElement->getTextContent()); // If no equiv specified, then displayed value = device's internal value, right?
				}
				attr.equivValues[equiv]
					= XMLString::transcode(currentElement->getTextContent());
				attr.dispToCmd[XMLString::transcode(currentElement->getTextContent())]
					= equiv;
			}
		}
	}

	(*devAttributes)[attr.name] = attr;
}

// Simple find-and-replace for command arguments.
string GenericDeviceConfig::commandSub(string cmdstr, const MixedValue& valueIn)
{
	int nargs = 1;
	if (valueIn.getType() == MixedValue::Vector)
		nargs = valueIn.getVector().size(); // If a vector, get vector size
	else if (valueIn.getType() == MixedValue::Empty)
		nargs = 0; // If empty

	if (!nargs) {
		return cmdstr;
	} else if (nargs == 1) {
		string placeholder = "$1";
		int ploc = cmdstr.find(placeholder);
		if (ploc != string::npos) cmdstr.replace(ploc, placeholder.size(), valueIn.print());

		return cmdstr;
	}

	// Deal with vectors
	for (int i = 0; i < nargs; i++) {
		string placeholder = "$";
		placeholder.append(std::to_string((long double)(i+1)));
		int ploc = cmdstr.find(placeholder);
		if (ploc != string::npos) cmdstr.replace(ploc, placeholder.size(), valueIn.getVector().at(i).print());
	}

	return cmdstr;
}

vector<Command>* GenericDeviceConfig::getCommands(int chan, int nargs)
{
	// FIXME: Error checking!
	if (channels->at(chan).routines.count(nargs)) {
		return &channels->at(chan).routines.at(nargs);
	}

	return NULL;
}

void GenericDeviceConfig::parseDeviceSpecificConfig(ConfigFile configFile)
{
	// NB: make sure we're called after deviceType gets set.
	if (deviceType == TYPE_RS232) {
		string tmp;
		// NB: OS-specific serial port should be used in config file.
		configFile.getParameter("Addr", serialSettings->portStr);

		configFile.getParameter("Parity", tmp);
		istringstream(tmp) >> serialSettings->parity;
		tmp.clear();

		configFile.getParameter("BaudRate", tmp);
		istringstream(tmp) >> serialSettings->baudRate;
		tmp.clear();

		configFile.getParameter("DataBits", tmp);
		istringstream(tmp) >> serialSettings->dataBits;
		tmp.clear();

		configFile.getParameter("StopBits", tmp);
		istringstream(tmp) >> serialSettings->stopBits;
	}
}

//std::map<int, vector<Command>> GenericDeviceConfig::processCommands(
/**
 *  This function:
 *  - Tests the access and availability of the XML configuration file.
 *  - Configures the xerces-c DOM parser.
 *  - Reads and extracts the pertinent information from the XML config file.
 *
 *  @param in configFile The text string name of the HLA configuration file.
 */

void GenericDeviceConfig::readConfigFile(string& configFile)
        throw( std::runtime_error )
{
   // Test to see if the file is ok.
	struct stat fileStatus;

	errno = 0;
	if(stat(configFile.c_str(), &fileStatus) == -1) // ==0 ok; ==-1 error
	{
		cerr << "Got error. Need to catch somewhere! " << errno << endl;
		if( errno == ENOENT )      // errno declared by include file errno.h
			throw ( std::runtime_error("Path file_name does not exist, or path is an empty string.") );
		else if( errno == ENOTDIR )
			throw ( std::runtime_error("A component of the path is not a directory."));
		else if( errno == ELOOP )
			throw ( std::runtime_error("Too many symbolic links encountered while traversing the path."));
		else if( errno == EACCES )
			throw ( std::runtime_error("Permission denied."));
		else if( errno == ENAMETOOLONG )
			throw ( std::runtime_error("File can not be read\n"));
	}

	// Configure DOM parser.

	m_ConfigFileParser->setValidationScheme( XercesDOMParser::Val_Never );
	m_ConfigFileParser->setDoNamespaces( false );
	m_ConfigFileParser->setDoSchema( false );
	m_ConfigFileParser->setLoadExternalDTD( false );
	try
	{
		m_ConfigFileParser->parse( configFile.c_str() );

		// no need to free this pointer - owned by the parent parser object
		DOMDocument* xmlDoc = m_ConfigFileParser->getDocument();

		// Get the top-level element: NAme is "root". No attributes for "root"
      
		DOMElement* elementRoot = xmlDoc->getDocumentElement();
		if (!elementRoot) throw(std::runtime_error( "empty XML document" ));

		if (strcmp(XMLString::transcode(elementRoot->getTagName()), DEVICE_STR))
			throw(std::runtime_error( "Problem with XML document" ));

		if (elementRoot->hasAttribute(ATTR_name))
			name = XMLString::transcode(elementRoot->getAttribute(ATTR_name));
		else
			name = string(UNKNOWN_DEVICE);


		if (elementRoot->hasAttribute(ATTR_proto)) {
			string type = XMLString::transcode(elementRoot->getAttribute(ATTR_proto));
			if (type == TYPE_GPIB_STR) {
				deviceType = TYPE_GPIB;
				gpibSettings = new GPIBSettings;

				// Workaround for ancient pAMeter (+others?)
				
				if (elementRoot->hasAttribute(ATTR_readtype)) {
					string tmp;
					istringstream(XMLString::transcode(elementRoot->getAttribute(ATTR_readtype)))
						>> tmp;
					if (tmp == READTYPESTR_SINGLECHAR) {
						gpibSettings->readType = READTYPE_SINGLECHAR;
					} else if (tmp.length() == 1 && tmp.at(0) >= '0' && tmp.at(0) <= '9') {
						// See gpibControllerDevice.cpp and GPIB_device.cpp for more on this.
						// Hacky stupid ugh, we're storing this as an 8-bit int.
						gpibSettings->readType = tmp.at(0) - '0';
					}
				} else  {
					gpibSettings->readType = READTYPE_STR;
				}

				

			} else if (type == TYPE_RS232_STR) {
				deviceType = TYPE_RS232;
				serialSettings = new SerialSettings;

				if (elementRoot->hasAttribute(ATTR_echo)) {
					istringstream(XMLString::transcode(elementRoot->getAttribute(ATTR_echo)))
						>> serialSettings->echo;
				} else serialSettings->echo = false;

				if (elementRoot->hasAttribute(ATTR_charDelay)) {
					istringstream(XMLString::transcode(elementRoot->getAttribute(ATTR_charDelay)))
						>> serialSettings->charDelay;
				} else serialSettings->charDelay = 0;

				if (elementRoot->hasAttribute(ATTR_querySleep)) {
					istringstream(XMLString::transcode(elementRoot->getAttribute(ATTR_querySleep)))
						>> serialSettings->querySleep;
				} else serialSettings->querySleep = RS232QUERYSLEEP_DEFAULT;
			}
		}

		// Parse XML file for tags of interest: "ApplicationSettings"
		// Look one level nested within "root". (child of root)

		DOMNodeList*      children = elementRoot->getChildNodes();
		const  XMLSize_t nodeCount = children->getLength();
		// For all nodes, children of "root" in the XML tree.

		for( XMLSize_t xx = 0; xx < nodeCount; ++xx )
		{
			DOMNode* currentNode = children->item(xx);
			if( currentNode->getNodeType() &&  // true is not NULL
				currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
			{
				// Found node which is an Element. Re-cast node as element
				DOMElement* currentElement
							= dynamic_cast< xercesc::DOMElement* >( currentNode );
				if( XMLString::equals(currentElement->getTagName(), TAG_channels))
				{
					processChannels(currentElement);
				} else if (XMLString::equals(currentElement->getTagName(), TAG_init)){
					initRoutine = processRoutine(currentElement);
				} else if (XMLString::equals(currentElement->getTagName(), TAG_attrs)){
					processAttributes(currentElement);
				} else if (XMLString::equals(currentElement->getTagName(), TAG_partners)) {
					processPartners(currentElement);
				}
			}
		}
	}
	catch( xercesc::XMLException& e )
	{
		char* message = xercesc::XMLString::transcode( e.getMessage() );
		ostringstream errBuf;
		errBuf << "Error parsing file: " << message << flush;
		XMLString::release( &message );
	}
}