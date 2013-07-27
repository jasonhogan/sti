#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP
/**
 *  @file
 *  Class "GenericDeviceConfig" provides the functions to read the XML data.
 *  @version 1.0
 */

// Handle device-specific .ini files.
#include "ConfigFile.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <string>
#include <stdexcept>

#include <vector>
#include <map>
#include <MixedValue.h>
// Define our tags and attributes
#define DEVICE_STR "device"
#define CHANNEL_STR "channel"
#define CHANNELS_STR "channels"
#define CMD_STR "command"
#define MANUF_STR "manufacturer"
#define NAME_STR "name"
#define PRETTYNAME_STR "prettyname"
#define ROUTINE_STR "routine"
#define NARGS_STR "nargs"
#define OUTPUT_STR "output"
#define INIT_STR "init"
#define ATTR_STR "attribute"
#define ATTRS_STR "attributes"
#define VALUE_STR "value"
#define PROTO_STR "proto"

#define UNKNOWN_DEVICE "Unknown device!"

/* CLEAN THIS UP: Move to header, remove altogether, etc. */
#define TYPE_GPIB 1
#define TYPE_RS232 2
#define TYPE_UNKNOWN 0

#define TYPE_GPIB_STR "GPIB"
#define TYPE_RS232_STR "RS232"

// Error codes

enum {
   ERROR_ARGS = 1, 
   ERROR_XERCES_INIT,
   ERROR_PARSE,
   ERROR_EMPTY_DOCUMENT
};

struct Command {
	std::string cmdstr; // Command string to run.
	bool output; // Do we return this value?
};

struct Channel {
	std::string name;
	std::string prettyname;
	std::map<int, std::vector<Command>> routines; // Map nargs to command (for overloading channels)
};

/* Maybe? struct DevAttributeValue {
	std::string value; // What value we get/set
	std::string equiv; // Equivalent value (might get returned)
	std::string displayValue; // TODO: we should display something prettier (1 -> On, True, etc.)
};*/

struct DevAttribute {
	std::string name;
	std::string prettyname;
	Command command;
	// FIXME: Allow for different returned values!
	std::vector<std::string> values;
	std::map<std::string, std::string> equivValues; // Map of equiv to value
};

struct SerialSettings {
	std::string portStr;
	unsigned int baudRate;
	unsigned int dataBits;
	unsigned int stopBits;
	std::string parity;
};
class GenericDeviceConfig
{
public:
	GenericDeviceConfig();
	~GenericDeviceConfig();
	void readConfigFile(std::string&) throw(std::runtime_error);
	std::vector<Command>* getCommands(int chan, int nargs);
	Command GenericDeviceConfig::processCommand(xercesc::DOMElement* currentElement);

	// Attribute duders
	void GenericDeviceConfig::processAttribute(xercesc::DOMElement* xattr);
	std::vector<DevAttribute>* getAttributes();
	bool getAttributeLabel(std::string name, std::string value, std::string* newLabel);

	char *getName() { return m_ATTR_name; };
	char *getPrettyname() { return m_ATTR_prettyname; };
	// TODO: Make these private & use getters/setters

	std::string name;
	std::vector<Channel>* channels;
	std::map<std::string, DevAttribute>* devAttributes;
	unsigned int numChannels() { return channels->size(); }

	std::vector<std::string> channelNames;
	std::vector<std::string> channelCmds;

	//xercesc::DOMElement* processCommand(xercesc::DOMElement*);
	void GenericDeviceConfig::processChannel(xercesc::DOMElement*);
	std::vector<Command> processRoutine(xercesc::DOMElement*);
	std::vector<Command> initRoutine;

	static std::string commandSub(std::string cmdstr, const MixedValue& valueIn);

	SerialSettings* serialSettings;
	int getDeviceType() { return deviceType; }

	void GenericDeviceConfig::parseDeviceSpecificConfig(ConfigFile configFile);

private:
	xercesc::XercesDOMParser *m_ConfigFileParser;
	char* m_ATTR_name;
	char* m_ATTR_prettyname;
	int deviceType;
	void GenericDeviceConfig::processChannels(xercesc::DOMElement* xchannels);
	void GenericDeviceConfig::processAttributes(xercesc::DOMElement* xattributes);

	// Internal class use only. Hold Xerces data in UTF-16 SMLCh type.

	XMLCh* TAG_device;
	XMLCh* TAG_channel;
	XMLCh* TAG_channels;
	XMLCh* TAG_cmd;
	XMLCh* TAG_routine;
	XMLCh* TAG_init;
	XMLCh* TAG_attr;
	XMLCh* TAG_attrs;
	XMLCh* TAG_value;

	XMLCh* ATTR_name;
	XMLCh* ATTR_prettyname;
	XMLCh* ATTR_nargs;
	XMLCh* ATTR_output;
	XMLCh* ATTR_equiv;
	XMLCh* ATTR_proto;
};
#endif