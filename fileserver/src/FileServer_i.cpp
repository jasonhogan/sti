/*! \file FileServer_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class FileServer_i
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

#include "FileServer_i.h"

#if defined(_MSC_VER)
	#define STI_USE_WINDOWS_READONLY
#endif

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>


FileServer_i::FileServer_i(std::string HomeDirectory)
{
	// Get native path separator
	fs::path pathSeparator("/", fs::native);
	nativePathSeparator = pathSeparator.native_directory_string();

	if( exists(HomeDirectory.c_str()) )
	{
		homeDirectory_l = fs::system_complete( fs::path(HomeDirectory)).native_directory_string();
	}
	else
	{
		std::cerr << "Home directory '" << HomeDirectory << "' not found or invalid." << std::endl;
		homeDirectory_l = ".";

	}
}

FileServer_i::~FileServer_i()
{
}

::CORBA::Boolean FileServer_i::isReadOnly(const char* path)
{
	std::string readonlyResultFileName = "STI_FileServer_ReadOnlyResult.tmp";
	std::stringstream command;
	
	if( exists(path) )
	{
#if defined(STI_USE_WINDOWS_READONLY)
		command << "attrib " << path << " > " << readonlyResultFileName;
		system( command.str().c_str() );
		
		fs::path fromPath(readonlyResultFileName, fs::native);
		fs::ifstream file( fromPath, std::ios_base::in | std::ios_base::binary );
		if ( file ) 
		{ 
			std::string line;
			while(getline(file, line))
			{
//				std::cout << "READONLY: " << line << std::endl;
			}
		}

#else
		return false;
#endif
	}
	return false;
}

char* FileServer_i::readData(const char* path)
{
	std::stringstream data;
	
	fs::path fromPath(path, fs::native);
	fs::ifstream file( fromPath, std::ios_base::in | std::ios_base::binary );
    
	if ( file ) 
	{ 
		std::string line;
		while(getline(file, line))
		{
//			std::cerr << line << std::endl;
			data << line << std::endl;
		}
	}
	else
	{
		std::cout << "Could not open file " << path << " for reading." << std::endl;
		data << "";
	}

	CORBA::String_var sendData( data.str().c_str() );
	return sendData._retn();
}

::CORBA::Boolean FileServer_i::writeData(const char* path, const char* data)
{
	if( isReadOnly(path) || isDirectory(path) )
		return false;

	fs::path writePath(path, fs::native);

	fs::ofstream writeFile( writePath, std::ios_base::out | std::ios_base::binary );

	if ( !writeFile )
	{
		std::cout << "Could not open file " << path << " for writting." << std::endl;
		return false;
	}

	writeFile << data;
	return true;

}

char* FileServer_i::normalize(const char* path)
{
	fs::path inputPath(path, fs::native);
	CORBA::String_var outputPath( inputPath.normalize().string().c_str() );
	return outputPath._retn();
}

char* FileServer_i::canonicalize(const char* path)
{
	fs::path inputPath(path, fs::native);
	CORBA::String_var outputPath( inputPath.canonize().string().c_str() );
	return outputPath._retn();
}

char* FileServer_i::getSeparator()
{
	CORBA::String_var separator( nativePathSeparator.c_str() );
	return separator._retn();
}


::CORBA::Boolean FileServer_i::isAbsolute(const char* path)
{
	fs::path inputPath(path, fs::native);
	return (fs::exists(inputPath) && inputPath.is_complete());
}

::CORBA::LongLong FileServer_i::getFileLength(fs::path full_path)
{
	boost::uintmax_t length = 0;	
	if(fs::exists(full_path) && fs::is_regular(full_path) )
		length = fs::file_size(full_path);

	return static_cast<::CORBA::LongLong>(length);
}

::CORBA::LongLong FileServer_i::getFileLength(const char* path)
{
	fs::path full_path(path, fs::native);
	return getFileLength(full_path);
}

::CORBA::LongLong FileServer_i::getLastWriteTime(fs::path full_path)
{
	time_t time = 0;
	if(fs::exists(full_path))
		time = fs::last_write_time(full_path);

	return static_cast<::CORBA::LongLong>(time);
}

::CORBA::LongLong FileServer_i::getLastWriteTime(const char* path)
{
	fs::path full_path(path, fs::native);
	return getLastWriteTime(full_path);
}

::CORBA::Boolean FileServer_i::exists(const char* file)
{
	fs::path full_path(file, fs::native);
	try {
		return (fs::exists(full_path));
	}
	catch( const std::exception & ) {
		return false;
	}
}

::CORBA::Boolean FileServer_i::isFile(const char* file)
{
	fs::path full_path(file, fs::native);
	try {
		return (fs::exists(full_path) && fs::is_regular(full_path));
	} 			
	catch( const std::exception & ) {
		return false;
	}
}

::CORBA::Boolean FileServer_i::isDirectory(const char* file)
{
	fs::path full_path(file, fs::native);
	try {
		return (fs::exists(full_path) && fs::is_directory(full_path));
	} 			
	catch( const std::exception & ) {
		return false;
	}
}

Remote_File_Server::TFileSeq* FileServer_i::getFiles(const char* dir)
{
	using Remote_File_Server::TFileSeq;
	using Remote_File_Server::TFile;


//	std::cerr << "getFiles(): " << dir << std::endl;

	unsigned i;

	std::vector<TFile> files;
/////
//	fs::path full_path( fs::initial_path<fs::path>() );

	fs::path full_path = fs::system_complete( fs::path( dir, fs::native ) );

	if ( fs::is_directory( full_path ) )
	{
//		std::cerr << "Is directory: "
//			<< full_path.native_directory_string() << "\n\n";

		fs::directory_iterator end_iter;
		for ( fs::directory_iterator dir_itr( full_path ); dir_itr != end_iter;	++dir_itr )
		{
			try
			{
				files.push_back(TFile());

				if ( fs::is_directory( dir_itr->status() ) )
				{
					files.back().filename = CORBA::String_var( dir_itr->path().native_file_string().c_str() );
					files.back().isDirectory = true;
//					std::cout << dir_itr->path().native_file_string() << " [directory]\n";
				}
				else if ( is_regular( dir_itr->status() ) )
				{
					files.back().filename = CORBA::String_var( dir_itr->path().native_file_string().c_str() );
					files.back().exists = true;
					files.back().isDirectory = false;
					files.back().length = static_cast<CORBA::ULong>(file_size(dir_itr->path()));
//					std::cout << dir_itr->path().native_file_string() << "\n";
				}
				else
				{
					files.back().exists = false;
					files.back().isDirectory = false;
//					std::cout << dir_itr->path().native_file_string() << " [other]\n";
				}
				

			}
			catch ( const std::exception & ex )
			{
				std::cout << dir_itr->path().native_file_string() << " " << ex.what() << std::endl;
			}
		}
	}

/////

	Remote_File_Server::TFileSeq_var fileSeq( new TFileSeq );
	fileSeq->length(files.size());

	for(i = 0; i < files.size(); i++)
	{
		fileSeq[i].filename = CORBA::String_var( files[i].filename );
		fileSeq[i].exists = files[i].exists;
		fileSeq[i].length = files[i].length;
		fileSeq[i].isDirectory = files[i].isDirectory;
		fileSeq[i].isHidden = false;

	}
	return fileSeq._retn();
}

char* FileServer_i::homeDirectory()
{
	fs::path home(homeDirectory_l, fs::native);

	CORBA::String_var homeDir( home.native_directory_string().c_str() );
	return homeDir._retn();
}

char* FileServer_i::getAbsolutePath(const char* path)
{
	fs::path relativePath(path, fs::native);

	std::string absolutePath;

	if(fs::exists(relativePath))
	{
		absolutePath = (fs::system_complete(relativePath)).native_file_string();
	}
	else
	{
		// not sure what this should return if the path doesn't exist
		absolutePath = path;
	}
	
	CORBA::String_var absolute( absolutePath.c_str() );
	return absolute._retn();
}

::CORBA::Boolean FileServer_i::createNewFolder(const char* containingDir, const char* filename)
{
	return true;
}

Remote_File_Server::TFile* FileServer_i::getParentFile(const Remote_File_Server::TFile& child)
{
	using Remote_File_Server::TFile;
	using Remote_File_Server::TFile_var;

	fs::path full_path = fs::system_complete( fs::path( child.filename, fs::native ) );

//	std::cerr << "getParent: " << full_path.branch_path().native_directory_string() << std::endl;

	TFile_var parent = new TFile();
	
	parent->filename = CORBA::String_var( full_path.branch_path().native_directory_string().c_str() );
	
	
//std::cerr << "1" << std::endl;
	parent->exists = fs::exists(full_path.branch_path());
//std::cerr << "2" << std::endl;

	parent->isDirectory = fs::is_directory(full_path.branch_path());
//std::cerr << "3" << std::endl;
//	parent->length = fs::file_size(full_path.branch_path());
//std::cerr << "4" << std::endl;

//std::cerr << "getParent: " << parent->filename << std::endl;

	parent->isHidden = false;
	parent->lastModified = 0;



	return parent._retn();


}

