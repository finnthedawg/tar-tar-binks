#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include "common.h"

/* Recursive iteration through directories in disk and add the information about the file/directory to
    the metadata vector and Immediately write to archive file on disk if its a file */

// TODO Softlinks and Hardlinsk
void iterate_through_dir(std::string baseDirName,
                         std::fstream &archivePtr,
                         struct Header &mainHeader,
                         std::vector <struct Metadata> &metaVector,
                         char flag) {
	DIR* dirPtr;                   // for opening a directory stream

	/* if there is an error in opening the dir stream */
	dirPtr = opendir(baseDirName.c_str());
	if (dirPtr == nullptr) {
		std::cerr << "ERROR <" << errno <<  "> Unable to open " << baseDirName << std::endl;
	}
	else {
		if (DEBUG) std::cout << "DEBUG " << "Opening " << baseDirName << std::endl;
		struct dirent *dirReadPointer; // pointer for reading the DIR stream
		dirReadPointer = readdir(dirPtr);

		while (dirReadPointer) {
			std::string fileName = dirReadPointer->d_name;
			/* excluding current dir and parent directory specifier . and .. along with DS_Store */
			if ( fileName != std::string(".") && fileName != std::string("..") &&
			     fileName != std::string(".DS_Store") ) {

				std::string pathToObject = baseDirName+"/"+fileName;
				if (dirReadPointer->d_type == DT_REG) {      // if dirReadPointer is a file
					mainHeader.fileCount++;

					if (DEBUG) std::cout << "DEBUG: name of file=" << fileName << std::endl;
					append_to_metadata(fileName, pathToObject, metaVector, mainHeader, archivePtr, flag);
				}
				else if (dirReadPointer->d_type == DT_DIR) { // if dirReadPointer is a directory
					mainHeader.directoryCount++;
					if (DEBUG) std::cout << "DEBUG: name of directory=" << fileName<< std::endl;
					// TODO
					update_metadata_in_memory(mainHeader, fileName, pathToObject, metaVector, archivePtr, flag);
					iterate_through_dir(pathToObject, archivePtr, mainHeader, metaVector, flag);
				}
			}
			dirReadPointer = readdir(dirPtr);
		}
		closedir(dirPtr);
	}
}

// /* Read Header from disk */
// struct Header read_header_from_disk(std::fstream archivePtr) {
//  // TODO read the size of long long to read the offsetToMeta
//  // TODO read the size of int to read the number of files & folders
//  struct Header mainReadHeader;
//  return mainReadHeader;
// }

/* Write header to disk */
int write_header_to_disk(struct Header &mainHeader,
                         std::fstream &archivePtr) {
	if (archivePtr.is_open()) {
		if (DEBUG==0) std::cout << "DEBUG Current ptr loc in archivePtr is " << archivePtr.tellg() <<" "<< archivePtr.tellp() << std::endl;
		archivePtr.clear();                 // clear the eof flag if it has been set for archivePtr
		if (DEBUG==0) std::cout << "DEBUG Current ptr loc in archivePtr is " << archivePtr.tellg() <<" "<< archivePtr.tellp() << std::endl;
		archivePtr.seekp(0, std::ios::beg); // reset the archivePtr to the start of the archive file
		if (DEBUG==0) std::cout << "DEBUG Current ptr loc in archivePtr is " << archivePtr.tellg() <<" "<< archivePtr.tellp() << std::endl;
		int mainHeaderSize = sizeof(mainHeader);
		std::cout << mainHeader.offsetToMeta << " " << mainHeader.fileCount << " " << mainHeader.directoryCount << std::endl;

		// fwrite(&mainHeader, sizeof(Header), 1, archivePtr);
		if (DEBUG==0) std::cout << "DEBUG Current ptr loc in archivePtr is " << archivePtr.tellg() <<" "<< archivePtr.tellp() << std::endl;
		archivePtr.seekp(0, std::fstream::beg);
		archivePtr.write((char *)"1234567891234567", mainHeaderSize);
		// archivePtr.write((char *)(&mainHeader), mainHeaderSize);

		if (DEBUG==0) std::cout << "DEBUG Current ptr loc in archivePtr is " << archivePtr.tellg() <<" "<< archivePtr.tellp() << std::endl;
		std::cout << "main hdr size is " << mainHeaderSize << '\n';
		if (DEBUG==0) std::cout << "DEBUG Current ptr loc in archivePtr is " << archivePtr.tellg() <<" "<< archivePtr.tellp() << std::endl;
		std::cout << "Printing " << (char*)&mainHeader << std::endl;
		archivePtr.seekp(0, std::fstream::beg);
		// archivePtr.write((mainHeader.offsetToMeta), sizeof(mainHeader.offsetToMeta));
		// archivePtr.write((mainHeader.fileCount), sizeof(mainHeader.fileCount));
		// archivePtr.write((mainHeader.directoryCount), sizeof(main.directoryCount));
	}
	else {
		std::cerr << "Failed to open archive file for writing header info" << std::endl;
		return -1;
	}
	return 0;
}

// TODO TODO TODO TODO TODO TODO TODO
/* Read Metadata from disk */
std::vector <struct Metadata> read_metadata_from_disk(std::fstream archivePtr,
                                                      struct Header mainHeader){
	archivePtr.seekg(mainHeader.offsetToMeta);    // Seek to the offset of metadata Header.offsetToMeta
	std::vector <struct Metadata> metaVector;
	while (archivePtr.is_open()) {
		struct Metadata meta;
		archivePtr.read((char *)&meta, sizeof(Metadata));
		metaVector.push_back(meta);
	}
	return metaVector;
}

//  TODO TODO TODO TODO TODO TODO
/* Write Metadata to disk */
/* Call this function at the very end */
int write_metadata_to_disk(struct Header &mainHeader,
                           std::fstream &archivePtr,
                           std::vector <struct Metadata> &metaVector){
	archivePtr.seekp(mainHeader.offsetToMeta);
	// TODO for each meta
	for(std::vector<int>::size_type i = 0; i != metaVector.size(); i++) {
		// TODO write to mainHeader.offsetToMeta
		int MetadataSize = sizeof(Metadata);
		archivePtr.write((char *)(&metaVector[i]), MetadataSize);
	}
	return 0;
}

/* Create a new Metadata object */
struct Metadata create_Metadata_object(struct Header &mainHeader,
                                       std::string fileName,
                                       std::string pathToObject) {
	// Getting the information about the file/directory
	struct stat fileStat;
	if (stat((pathToObject).c_str(), &fileStat) == -1) {
		std::cerr << "ERROR <" << errno << "> Unable to get stat info on " << pathToObject << '\n';
	}

	std::string userID, groupID, mode,
	            size, accessDate, modifyDate,
	            changeDate, birthDate, inode;
	int numHardLinks;

	userID = std::to_string(fileStat.st_uid);
	groupID = std::to_string(fileStat.st_gid);
	mode = std::to_string(fileStat.st_mode);
	size = std::to_string(fileStat.st_size);
	accessDate = std::to_string(fileStat.st_atime);
	modifyDate = std::to_string(fileStat.st_mtime);
	changeDate = std::to_string(fileStat.st_ctime);
	birthDate = std::to_string(fileStat.st_birthtime);
	inode = std::to_string(fileStat.st_ino);
	numHardLinks = (int)fileStat.st_nlink;

	if (DEBUG) {
		std::cout << "File name: " << pathToObject << std::endl;
		std::cout << "User ID: " << fileStat.st_uid << std::endl;
		std::cout << "Group ID: " << fileStat.st_gid << std::endl;
		std::cout << "Mode: " << fileStat.st_mode << std::endl;
		std::cout << "Size: " << fileStat.st_size << std::endl;
		std::cout << "Number of hard links to the file: " << fileStat.st_nlink << std::endl;
		std::cout << "Inode number: " << fileStat.st_ino << std::endl;
		std::cout << "Last access date: " << fileStat.st_atime << std::endl;
		std::cout << "Last modification date: " << fileStat.st_mtime << std::endl;
		std::cout << "Last change date: " << fileStat.st_ctime << std::endl;
		std::cout << "Birth / creation date: " << fileStat.st_birthtime << std::endl;
	}

	Metadata currentMeta;
	// if the object in question is a directory
	if (S_ISDIR(fileStat.st_mode)) {
		currentMeta.directory = 1;
		currentMeta.file = 0;
		currentMeta.offsetToFileStart = -1; // -1 for directories
	}
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO
	// additional checks for symbolic links, sockets, pipes or devices
	// then it is a file
	else {
		currentMeta.file = 1;
		currentMeta.directory = 0;
		currentMeta.offsetToFileStart = mainHeader.offsetToMeta;
	}
	currentMeta.version = 1; // Can be updated when the -a flag is used
	currentMeta.numberOfLinks = numHardLinks;
	strncpy(currentMeta.inode, (inode+"\0").c_str(), sizeof(inode)+1);
	strncpy(currentMeta.fileSize, (size+"\0").c_str(), sizeof(size)+1);
	strncpy(currentMeta.userID, (userID+"\0").c_str(), sizeof(userID)+1);
	strncpy(currentMeta.groupID, (groupID+"\0").c_str(), sizeof(groupID)+1);
	strncpy(currentMeta.filePermission, (mode+"\0").c_str(), sizeof(mode)+1);
	strncpy(currentMeta.fileName, (fileName+"\0").c_str(), sizeof(fileName)+1);
	strncpy(currentMeta.birthDate, (birthDate+"\0").c_str(), sizeof(birthDate)+1);
	strncpy(currentMeta.accessDate, (accessDate+"\0").c_str(), sizeof(accessDate)+1);
	strncpy(currentMeta.modifyDate, (modifyDate+"\0").c_str(), sizeof(modifyDate)+1);
	strncpy(currentMeta.changeDate, (changeDate+"\0").c_str(), sizeof(changeDate)+1);
	strncpy(currentMeta.pathToObject, (pathToObject+"\0").c_str(), sizeof(pathToObject)+1);
	/*
	   currentMeta.userID = (userID+"\0").c_str();
	   currentMeta.groupID = (groupID+"\0").c_str();
	   currentMeta.fileName = (fileName+"\0").c_str();
	   currentMeta.birthDate = (birthDate+"\0").c_str();
	   currentMeta.filePermission = (mode+"\0").c_str();
	   currentMeta.accessDate = (accessDate+"\0").c_str();
	   currentMeta.modifyDate = (modifyDate+"\0").c_str();
	   currentMeta.changeDate = (changeDate+"\0").c_str();
	   currentMeta.pathToObject = (pathToObject+"\0").c_str();
	 */

	return currentMeta; // return the populated Metadata object
}

/* Add updated version or new Meta struct to Metadata in memory*/
/* When using -a append flag, search through Global Metadata Struct to see if a prev ver exists
   Push an updated version meta to vector if found
   Push a new meta to vector if not found
   For flag = c, push to metaVector directly without checking for old versions */
int update_metadata_in_memory( struct Header &mainHeader,
                               std::string fileName,
                               std::string pathToObject,
                               std::vector <struct Metadata> &metaVector,
                               std::fstream &archivePtr,
                               char flag) {
	/* create and populate a new Metadata object */
	Metadata currentMeta;
	currentMeta = create_Metadata_object(mainHeader, fileName, pathToObject);

	/* if flag is 'a' then search through the Global Metadata Struct to update the version
	   if not found, push to vector */
	if (flag == 'a') {
		/*TODO TODO TODO TODO TODO*/
		;
	}
	/* if flag is 'c' add to vector directly */
	else if (flag == 'c') {
		metaVector.push_back(currentMeta);
	}
	return 0;
}

/* Append to Metadata in memory and Add New Files to archive file on disk
   This function should only be called for files not directories */
int append_to_metadata(std::string fileName,
                       std::string pathToObject,
                       std::vector <struct Metadata> &metaVector,
                       struct Header &mainHeader,
                       std::fstream &archivePtr,
                       char flag){
	if (DEBUG) std::cout << "Filename and size is " << fileName << "  size: " << sizeof(fileName) << '\n';
	if (DEBUG) std::cout << "Path to object name is" << pathToObject << " size: " << sizeof(pathToObject) <<  '\n';
	/* Search through global metadata Struct to update version.
	   Push to vector if not found
	   if -c flag is used, (i.e. char flag='c'), do not check for version,
	   but add to metaVector directly */
	update_metadata_in_memory(mainHeader, fileName, pathToObject, metaVector, archivePtr, flag);

	// write the currently read file to disk
	append_file_to_disk(archivePtr, pathToObject, mainHeader);
	return 0;
}

/* Utility functions */
/* Append/Write the file to the archive on disk for -c CREATE and -a APPEND flag */
int append_file_to_disk(std::fstream &archivePtr,
                        std::string pathToObject,
                        struct Header &mainHeader
                        ) {
	/* appends file to current pffsetToMeta */
	std::ifstream readFile;  // open a input file stream

	readFile.open(pathToObject, std::ios::binary);
	if (DEBUG==0) std::cout << "DEBUG 1 Current FILE writer ptr loc in archivePtr is " << archivePtr.tellp() << std::endl;
	archivePtr.clear();
	archivePtr.seekp(mainHeader.offsetToMeta); // set pointer to the start of the offsetToMeta
	if (DEBUG==0) std::cout << "DEBUG   2 Current FILE writer ptr loc in archivePtr is " << archivePtr.tellp() << std::endl;

	if (readFile.is_open()) {
		char buffer;             // buffer for reading from pathToObject and writing to readFile
		while (!readFile.eof()) {
			buffer = (char) readFile.get();
			archivePtr.put(buffer);
			// Update the offsetToMeta offset
			mainHeader.offsetToMeta += sizeof(buffer);
			std::cout << "APtr = " << archivePtr.tellg() << '\n';
		}
		readFile.close();
	}
	else {
		// cennot open file
		std::cerr << "ERROR <" << errno << "> Failed to open file "
		          << pathToObject << " for writing." << std::endl;
		return -1;
	}
	if (DEBUG==0) std::cout << "DEBUG     3 Current FILE writer ptr loc in archivePtr is " << archivePtr.tellp() << std::endl;
	return 0;
}

/* gets file size of a fstream object */
int file_size(std::fstream &fstream_obj){
	fstream_obj.seekg(0, fstream_obj.end);
	int length = fstream_obj.tellg();
	fstream_obj.seekg(0, fstream_obj.beg);
	return(length);
}
