#ifndef FILESERVER_I_H
#define FILESERVER_I_H

#include "FileServer.h"
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
namespace fs = boost::filesystem;

class FileServer_i : public POA_Remote_File_Server::FileServer
{
public:

	FileServer_i();
	virtual ~FileServer_i();

    virtual ::CORBA::Boolean exists(const char* file);
    virtual ::CORBA::Boolean isFile(const char* file);
    virtual ::CORBA::Boolean isDirectory(const char* file);
	virtual ::CORBA::Boolean isAbsolute(const char* path);

    virtual ::CORBA::Boolean createNewFolder(const char* containingDir, const char* filename);
	virtual Remote_File_Server::TFileSeq* getFiles(const char* dir);
	virtual Remote_File_Server::TFile* getParentFile(const Remote_File_Server::TFile& child);
    virtual char* homeDirectory();

    virtual char* normalize(const char* path);
    virtual char* canonicalize(const char* path);
    virtual char* getSeparator();

    virtual char* getAbsolutePath(const char* path);
    virtual ::CORBA::LongLong getFileLength(const char* path);
    virtual ::CORBA::LongLong getLastWriteTime(const char* path);

private:
	std::string nativePathSeparator;

	::CORBA::LongLong getFileLength(fs::path full_path);
    ::CORBA::LongLong getLastWriteTime(fs::path full_path);


};

#endif