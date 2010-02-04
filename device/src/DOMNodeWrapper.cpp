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

DOMNodeWrapper* DOMNodeWrapper::setAttribute(std::string key, std::string value)
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