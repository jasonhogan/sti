/*! \file FileServer_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class FileServer_i
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
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


#ifndef FILESERVER_I_H
#define FILESERVER_I_H

#include "FileServer.h"

#include <string>
#include <boost/filesystem.hpp>
#include <boost/progress.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;


class FileServer_i : public POA_Remote_File_Server::FileServer
{
public:

	FileServer_i(std::string HomeDirectory);
	virtual ~FileServer_i();

    ::CORBA::Boolean exists(const char* file);
    ::CORBA::Boolean isFile(const char* file);
    ::CORBA::Boolean isDirectory(const char* file);
	::CORBA::Boolean isAbsolute(const char* path);

    ::CORBA::Boolean createNewFolder(const char* containingDir, const char* filename);
	Remote_File_Server::TFileSeq* getFiles(const char* dir);
	Remote_File_Server::TFile* getParentFile(const Remote_File_Server::TFile& child);
    char* homeDirectory();

    char* normalize(const char* path);
    char* canonicalize(const char* path);
    char* getSeparator();

    char* getAbsolutePath(const char* path);
    ::CORBA::LongLong getFileLength(const char* path);
    ::CORBA::LongLong getLastWriteTime(const char* path);
    
	::CORBA::Boolean isReadOnly(const char* path);
    char* readData(const char* path);
    ::CORBA::Boolean writeData(const char* path, const char* data);

private:
	
	std::string nativePathSeparator;

	::CORBA::LongLong getFileLength(fs::path full_path);
    ::CORBA::LongLong getLastWriteTime(fs::path full_path);

	std::string homeDirectory_l;

};

#endif
