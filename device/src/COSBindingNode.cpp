/*! \file COSBindingNode.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class COSBindingNode
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

#include <COSBindingNode.h>
#include <omniORB4/omniURI.h>

#include <iostream>

COSBindingNode::COSBindingNode(std::string nodeName, bool isDeadLeaf)
{
	//By assumption, this is a leaf, since it has no context.
	name = nodeName;
	_isDead = isDeadLeaf;
	_isLeaf = true;
}

COSBindingNode::COSBindingNode(std::string nodeName, CosNaming::NamingContext_var& nodeContext)
{
	name = nodeName;
	_isDead = false;
	_isLeaf = false;
	
	walkBranches(nodeContext);
	
	//try {
	//}
	//catch(CORBA::TRANSIENT& ex)
	//{
	//	std::cerr << "walkBranches exception: " << ex._name() << std::endl;
	//	_isDead = true;
	//}
	//catch(CORBA::INV_OBJREF& ex)
	//{
	//	std::cerr << "walkBranches exception: " << ex._name() << std::endl;
	//	_isLeaf = true;
	//}
	//catch(CORBA::Exception& ex)
	//{
	//	std::cerr << "walkBranches exception: " << ex._name() << std::endl;
	//}
	//catch(...) 
	//{
	//	std::cerr << "Other exception." << std::endl;
	//}
}
COSBindingNode::COSBindingNode(const COSBindingNode& copy)
{
	_branches = copy._branches;
	name = copy.name;
	_isDead = copy._isDead;
	_isLeaf = copy._isLeaf;
}

COSBindingNode& COSBindingNode::operator= (const COSBindingNode& other)
{
	_branches = other._branches;
	name = other.name;
	_isDead = other._isDead;
	_isLeaf = other._isLeaf;

	return (*this);
}


COSBindingNode::~COSBindingNode()
{
//	std::cerr << "Deleting " << name << std::endl;
	for(unsigned i = 0; i < _branches.size(); i++)
	{
		delete (_branches.at(i));
	}
}

bool COSBindingNode::isDead() const
{
	return _isDead;
}

bool COSBindingNode::isLeaf() const
{
	return _isLeaf;
}

void COSBindingNode::prune()
{
}


void COSBindingNode::walkBranches(CosNaming::NamingContext_var& nodeContext)
{
	CosNaming::NamingContext_var newNodeContext;
	CosNaming::Binding_var binding( new CosNaming::Binding );

	CosNaming::BindingIterator_var biIterVar( new CosNaming::_objref_BindingIterator );
	CosNaming::BindingIterator_out biIter( biIterVar );

	CosNaming::BindingList_var biListVar( new CosNaming::BindingList );
	CosNaming::BindingList_out biList( biListVar );
	
	CORBA::Object_var obj;

	unsigned i = 0;
	
	if(CORBA::is_nil(nodeContext))
	{
//		std::cerr << "Found nil reference!" << std::endl;
		_isLeaf = true;
		
		//No need to iterate through the tree; this is a leaf.
		return;
	}
	
	try{
		nodeContext->list(0, biList, biIter);
	}
	catch(CORBA::TRANSIENT&)
	{
//		std::cerr << "list exception: " << ex._name() << std::endl;
		_isDead = true;
	}
	catch(CORBA::INV_OBJREF&)
	{
//		std::cerr << "list exception: " << ex._name() << std::endl;
		_isLeaf = true;
	}
	catch(CORBA::Exception&)
	{
//		std::cerr << "list exception: " << ex._name() << std::endl;
	}
	catch(...) 
	{
		std::cerr << "Other list exception." << std::endl;
	}

	if( isLeaf() )
	{
		//No need to iterate through the tree; this is a leaf.
		return;
	}

	bool deadServantFound = false;

	while(biIter->next_one(binding))
	{
		i++;
//		std::cerr << i << ". " << omni::omniURI::nameToString( binding->binding_name ) << std::endl;

		//get the context for this branch and add a new node
		obj = nodeContext->resolve( binding->binding_name );

		try {
			obj->_non_existent();
		}
		catch(CORBA::TRANSIENT&)
		{
			//This is a dead servant. 
			deadServantFound = true;
//			std::cerr << "_non_existent(): " << ex._name() << std::endl;
			_branches.push_back( 
				new COSBindingNode(
				omni::omniURI::nameToString( binding->binding_name ), true) );
		}

		//if( CORBA::is_nil(obj) || obj->_non_existent() || obj->_NP_is_nil() )
		//{
		//		std::cerr << "nil object" << std::endl;
		//	}

		if( !deadServantFound )
		{
			try {
				newNodeContext = CosNaming::NamingContext::
					_narrow( obj );
			}
			catch(...)
			{
				std::cerr << "Branch list exception: _narrow" << std::endl;
			}

			try {
				_branches.push_back( 
					new COSBindingNode(
					omni::omniURI::nameToString( binding->binding_name ), newNodeContext)
					);
			}
			catch(CORBA::INV_OBJREF&)
			{
				std::cerr << "Branch list exception: push_back" << std::endl;
			}
		}
	}
	//		delete biList;
	//		delete biIter;
	//	}
	//	catch(CORBA::Exception& ex)
	//	{
	//		std::cerr << "Branch list exception." << std::endl;
	//	}
}

std::string COSBindingNode::getName() const
{
	return name;
}

bool COSBindingNode::hasBranches() const
{
	return ( branches() > 0 );
}

unsigned int COSBindingNode::branches() const
{
	return _branches.size();
}

COSBindingNode& COSBindingNode::operator[] (unsigned int i) const
{
	return *( _branches.at(i) );
}

void COSBindingNode::printNode(unsigned int offset)
{
	unsigned i;
	
	for(i = 0; i < offset; i++)
		std::cerr << "*";
	std::cerr << name << " ";
	if( isLeaf() )
		std::cerr << (isDead() ? "(Dead)" : "(Alive)");
	std::cerr << std::endl;

	for(i = 0; i < branches(); i++)
	{
		(*this)[i].printNode(offset + 1);
	}
}

void COSBindingNode::printTree()
{
	printNode(1);
}

