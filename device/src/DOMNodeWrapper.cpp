/*! \file DOMNodeWrapper.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DOMNodeWrapper
 *  \section license License
 *
 *  Copyright (C) 2009 Jason Hogan <hogan@stanford.edu>\n
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

#include "DOMNodeWrapper.h"


#include <xstring.h>


DOMNodeWrapper::DOMNodeWrapper(DOMNode* node, DOMDocument* document): 
domNode(node), doc(document)
{
	type = Node;
}

DOMNodeWrapper::DOMNodeWrapper(DOMElement* node, DOMDocument* document) : 
domElement(node), doc(document)
{
	type = Element;
}

DOMNodeWrapper::~DOMNodeWrapper()
{
	for(unsigned i = 0; i < children.size(); i++)
	{
		delete children.at(i);
	}
}

//DOMNodeWrapper* DOMNodeWrapper::getRootNode()
//{
//}

DOMNodeWrapper* DOMNodeWrapper::appendChildElement(std::string name)
{
	DOMElement* newChild = doc->createElement( xstring(name).toXMLCh() );
	children.push_back( new DOMNodeWrapper(newChild, doc) );

	switch(type)
	{
	case Node:
		domNode->appendChild(newChild);
		break;
	case Element:
		domElement->appendChild(newChild);
		break;
	}

	return children.back();
}

DOMNodeWrapper* DOMNodeWrapper::appendTextNode(std::string text)
{
	DOMText* newChild = doc->createTextNode( xstring(text).toXMLCh() );
	children.push_back( new DOMNodeWrapper(newChild, doc) );

	switch(type)
	{
	case Node:
		domNode->appendChild(newChild);
		break;
	case Element:
		domElement->appendChild(newChild);
		break;
	}

	return children.back();
}

DOMNodeWrapper* DOMNodeWrapper::appendMixedDataNode(const MixedData& data)
{

	return addMixedDataToNode(this, data);

/*
	DOMText* newChild = doc->createTextNode( xstring(text).toXMLCh() );
	children.push_back( new DOMNodeWrapper(newChild, doc) );

	DOMNodeWrapper* returnNode;

	switch(type)
	{
	case Node:
		returnNode = addMixedDataToNode(domNode, data);
		break;
	case Element:
		returnNode = addMixedDataToNode(domElement, data);
		break;
	}

	return returnNode;
	*/
}
DOMNodeWrapper* DOMNodeWrapper::appendMixedValueNode(const MixedValue& value)
{
	return addMixedValueToNode(this, value);
}

DOMNodeWrapper* DOMNodeWrapper::setAttribute(const std::string& key, const std::string& value)
{
	if(domElement != NULL)
	{
		domElement->setAttribute( xstring(key).toXMLCh(), xstring(value).toXMLCh() );
	}

	return this;
}



DOMNode* DOMNodeWrapper::getDOMNode()
{
	switch(type)
	{
	case Node:
		return domNode;
	case Element:
		return domElement;
	}
	return NULL;
}

DOMElement* DOMNodeWrapper::getDOMElement()
{
	return domElement;
}

DOMNodeWrapper* DOMNodeWrapper::addMixedDataToNode(DOMNodeWrapper* measurementNode, const MixedData& data)
{
	DOMNodeWrapper* returnNode;

	switch(data.getType())
	{
	case MixedData::Boolean:
		returnNode = measurementNode->appendChildElement("bool")->appendTextNode( STI::Utils::valueToString(data.getBoolean()));
		break;
	case MixedData::Octet:
		returnNode = measurementNode->appendChildElement("octet")->appendTextNode( STI::Utils::valueToString(data.getOctet()));
		break;
	case MixedData::Double:
		returnNode = measurementNode->appendChildElement("double")->appendTextNode( STI::Utils::valueToString(data.getDouble()));
		break;
	case MixedData::Int:
		returnNode = measurementNode->appendChildElement("int")->appendTextNode( STI::Utils::valueToString(data.getInt()));
		break;
	case MixedData::String:
		returnNode = measurementNode->appendChildElement("string")->appendTextNode( STI::Utils::valueToString(data.getString()));
		break;
	case MixedData::File:
		returnNode = measurementNode->appendChildElement("file");
		//->appendTextNode( STI::Utils::valueToString(data));
		break;
	case MixedData::Vector:
		{
			DOMNodeWrapper* vecNode = measurementNode->appendChildElement("vector");
			for(unsigned i = 0; i < data.getVector().size(); i++)
			{
				returnNode = addMixedDataToNode(vecNode, data.getVector().at(i));
			}
		}
		break;
	default:
		returnNode = NULL;
		break;
	}

	return returnNode;
}

DOMNodeWrapper* DOMNodeWrapper::addMixedValueToNode(DOMNodeWrapper* measurementNode, const MixedValue& value)
{
	DOMNodeWrapper* returnNode;

	switch(value.getType())		//Boolean, Int, Double, String, Vector, Empty
	{
	case MixedValue::Boolean:
		returnNode = measurementNode->appendChildElement("bool")->appendTextNode( STI::Utils::valueToString(value.getBoolean()));
		break;
	case MixedValue::Int:
		returnNode = measurementNode->appendChildElement("int")->appendTextNode( STI::Utils::valueToString(value.getInt()));
		break;
	case MixedValue::Double:
		returnNode = measurementNode->appendChildElement("double")->appendTextNode( STI::Utils::valueToString(value.getDouble()));
		break;
	case MixedValue::String:
		returnNode = measurementNode->appendChildElement("string")->appendTextNode( STI::Utils::valueToString(value.getString()));
		break;
	case MixedValue::Vector:
		{
			DOMNodeWrapper* vecNode = measurementNode->appendChildElement("vector");
			for(unsigned i = 0; i < value.getVector().size(); i++)
			{
				returnNode = addMixedValueToNode(vecNode, value.getVector().at(i));
			}
		}
		break;
	case MixedValue::Empty:
		returnNode = measurementNode->appendChildElement("empty")->appendTextNode( "" );
		break;
	default:
		returnNode = NULL;
		break;
	}

	return returnNode;
}

