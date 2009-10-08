/*! \file COSBindingNode.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class COSBindingNode
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

//COS (Common Object Services) binding node tree object

#ifndef COSBINDINGNODE_H
#define COSBINDINGNODE_H

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB4/CORBA.h>
#endif

#include <vector>
#include <string>

class COSBindingNode;

class COSBindingNode
{
public:

	COSBindingNode(std::string nodeName, bool isDeadLeaf);
	COSBindingNode(std::string nodeName, CosNaming::NamingContext_var& nodeContext);
	~COSBindingNode();

	bool hasBranches() const;
	unsigned int branches() const;
	
	COSBindingNode& operator[] (unsigned int i) const;

	std::string getName() const;
	bool isDead() const;
	bool isLeaf() const;

	void prune();

	void printTree();

private:

	void walkBranches(CosNaming::NamingContext_var& nodeContext);

	void printNode(unsigned int offset);
	
	std::vector<COSBindingNode*> _branches;
	std::string name;
	bool _isDead;
	bool _isLeaf;

};

#endif

