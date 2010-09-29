/*! \file DOMNodeWrapper.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DOMNodeWrapper
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


#ifndef DOMNODEWRAPPER_H
#define DOMNODEWRAPPER_H

#include <string>
#include <vector>

//xerces
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include "MixedData.h"
#include "Utils.h"

XERCES_CPP_NAMESPACE_USE

class DOMNodeWrapper
{
public:

	DOMNodeWrapper(DOMNode* node, DOMDocument* document);
	DOMNodeWrapper(DOMElement* node, DOMDocument* document);
	~DOMNodeWrapper();

//	DOMNodeWrapper* getRootNode();
	DOMNodeWrapper* appendChildElement(std::string name);
	DOMNodeWrapper* appendTextNode(std::string text);
	DOMNodeWrapper* appendMixedDataNode(const MixedData& data);
	DOMNodeWrapper* setAttribute(const std::string& key, const std::string& value);

	DOMNode* getDOMNode();
	DOMElement* getDOMElement();

private:

	enum DOMType {Node, Element};

	DOMType type;
	
	DOMElement* domElement;
	DOMNode* domNode;


	DOMDocument* doc;
	std::vector<DOMNodeWrapper*> children;

	DOMNodeWrapper* addMixedDataToNode(DOMNodeWrapper* measurementNode, const MixedData& data);
};

#endif
