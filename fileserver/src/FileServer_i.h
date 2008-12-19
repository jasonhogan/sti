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

	FileServer_i();
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


};

#endif