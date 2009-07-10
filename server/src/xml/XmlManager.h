// Xml Manager class


#ifndef XMLMANAGER_H
#define XMLMANAGER_H

// c++ library includes
#include <string>
#include <vector>

// xerces parser includes
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>

// error handling utilites
#include "DOMTreeErrorReporter.hpp"
#include "DOMPrintErrorHandler.hpp"
#include <xercesc/util/OutOfMemoryException.hpp>

// output to file / screen functions
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/Xerces_autoconf_config.hpp>


class XmlManager {
public:
	XmlManager();

	~XmlManager();

	void CreateDocument();

	bool SetupWriter();

	void PrintDocumentToScreen();

	void PrintDocumentToFile(const char* const filename);

	void ImportDocument(const char* const filename);

	int errorCode;

	

protected:


private:
	
	bool errorsOccured;
	bool haveWriter;
	bool haveDocument;
	
	XERCES_CPP_NAMESPACE::DOMImplementation* documentImpl;

	XERCES_CPP_NAMESPACE::DOMImplementation *writerImpl;

	XERCES_CPP_NAMESPACE::DOMDocumentType* doctype;

	XERCES_CPP_NAMESPACE::DOMDocument* doc;

	DOMTreeErrorReporter *errReporter;


	XERCES_CPP_NAMESPACE::DOMLSSerializer *theSerializer;

	XERCES_CPP_NAMESPACE::XercesDOMParser *parser;

	XERCES_CPP_NAMESPACE::XMLFormatTarget *myFormTarget;
	XERCES_CPP_NAMESPACE::DOMLSOutput     *theOutputDesc;

	XMLCh tempStr[100];


	// options defined for the parser and the parser output - move location??

	bool                     gSplitCdataSections;
	bool                     gDiscardDefaultContent;

	bool                     gFormatPrettyPrint;
	bool                     gWriteBOM;

	bool                     gDoNamespaces;
	bool                     gDoSchema;
	bool                     gSchemaFullChecking;
	bool                     gDoCreate;

	

};

#endif