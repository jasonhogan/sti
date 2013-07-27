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
	TAG_routine = XMLString::transcode(ROUTINE_STR);
	TAG_init = XMLString::transcode(INIT_STR);
	TAG_attr = XMLString::transcode(ATTR_STR);
	TAG_attrs = XMLString::transcode(ATTRS_STR);
	TAG_value = XMLString::transcode(VALUE_STR);

	ATTR_proto = XMLString::transcode(PROTO_STR);
	ATTR_name = XMLString::transcode(NAME_STR);
	ATTR_prettyname = XMLString::transcode(PRETTYNAME_STR);
	ATTR_output = XMLString::transcode(OUTPUT_STR);
	ATTR_nargs = XMLString::transcode(NARGS_STR);
	ATTR_equiv = XMLString::transcode("equiv");

	deviceType = TYPE_UNKNOWN;

	channels = new std::vector<Channel>();
	devAttributes = new std::map<std::string, DevAttribute>();
	serialSettings = NULL;

	m_ConfigFileParser = new XercesDOMParser;
}

/**
 *  Class destructor frees memory used to hold the XML tag and 
 *  attribute definitions. It als terminates use of the xerces-C
 *  framework.
 */

GenericDeviceConfig::~GenericDeviceConfig()
{
   // Free memory
   delete m_ConfigFileParser;
   m_ConfigFileParser = NULL;
   delete channels;
   channels = NULL;
   delete devAttributes;
   devAttributes = NULL;
   delete serialSettings;
   serialSettings = NULL;
 
   if(m_ATTR_name)   XMLString::release( &m_ATTR_name );
   if(m_ATTR_prettyname)   XMLString::release( &m_ATTR_prettyname );

   try
   {
      XMLString::release( &TAG_device );

      XMLString::release( &TAG_channel );
      XMLString::release( &ATTR_name);
      XMLString::release( &ATTR_prettyname);
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

void GenericDeviceConfig::processAttributes(DOMElement* xattributes)
{
	DOMNodeList*      children = xattributes->getChildNodes();
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
			if(XMLString::equals(currentElement->getTagName(), TAG_attr)) {
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

bool GenericDeviceConfig::getAttributeLabel(std::string name, std::string value, std::string* newLabel)
{
	DevAttribute da = devAttributes->at(name);

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

	// Match numeric value, without regards to formatting.
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

	cerr << "Cannot find proper label." << endl;
	// Default is to just set to first value.
	*newLabel = da.values.at(0);

	return false;
}

void GenericDeviceConfig::processAttribute(DOMElement* xattr)
{
	DOMNodeList*      children = xattr->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	//child->getParentNode()->getNodeName
	DevAttribute attr;
	attr.name = XMLString::transcode(xattr->getAttribute(ATTR_name));
	attr.prettyname = XMLString::transcode(xattr->getAttribute(ATTR_prettyname));

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
				attr.command = processCommand(currentElement);
			} else if(XMLString::equals(currentElement->getTagName(), TAG_value)) {
				attr.values.push_back(XMLString::transcode(currentElement->getTextContent()));
				if (currentElement->hasAttribute(ATTR_equiv)) {
					attr.equivValues[XMLString::transcode(currentElement->getAttribute(ATTR_equiv))]
						= XMLString::transcode(currentElement->getTextContent());
				}
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
			if (type == TYPE_GPIB_STR)
				deviceType = TYPE_GPIB;
			else if (type == TYPE_RS232_STR)
				deviceType = TYPE_RS232;
				serialSettings = new SerialSettings;
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
				} else if( XMLString::equals(currentElement->getTagName(), TAG_init)){
					initRoutine = processRoutine(currentElement);
				} else if( XMLString::equals(currentElement->getTagName(), TAG_attrs)){
					processAttributes(currentElement);
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