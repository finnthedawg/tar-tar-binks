#include <pwd.h>
#include <grp.h>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <uuid/uuid.h>
#include "common.h"

/* function to add objects to archive works for both -c and -a flags */
int add_objects_to_archive(std::vector<std::string> inputList, std::fstream &archivePtr,
                           struct Header &mainHeader, std::vector<struct Metadata>& metaVector, char flag) {
	struct stat initialStat;

	// iterating through the inputList vector to get all the files/dirs to archive
	// iterate_through_dir write the contents of all files that have been found
	// starting from the mainHeader.offsetToMeta
	for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
		std::string fileName = get_filename_from_path(inputList[i]);
		if (DEBUG) std::cout << "DEBUG Printing file name without leading path " << fileName << '\n';
		if (DEBUG) std::cout << "DEBUG Starting archive with (fullpath) " << inputList[i] << std::endl;

		/* IMPORTANT this if check is only done once in the begining if there is only one main file or folder to archive */
		if (stat((inputList[i]).c_str(), &initialStat) == 0) {
			if (S_ISDIR(initialStat.st_mode)) {     // inputList[i] is a directory
				mainHeader.directoryCount++;
				if (DEBUG) std::cout << "DEBUG Name of directory = " << fileName<< std::endl;
				update_metadata_in_memory(mainHeader, fileName, inputList[i], metaVector, archivePtr, flag);
			}
			else if(S_ISREG(initialStat.st_mode)) { // inputList[i] is a regular file
				mainHeader.fileCount++;
				if (DEBUG) std::cout << "DEBUG Name of file = " << fileName << std::endl;
				append_to_metadata(fileName, inputList[i], metaVector, mainHeader, archivePtr, flag);
				continue; // continue iteration without calling iterate_through_dir if it is a file
			}
			else if(S_ISLNK(initialStat.st_mode)) { // inputList[i] is a symbolic link
				mainHeader.symboliclinkCount++;
				if (DEBUG) std::cout << "DEBUG Name of symbolic link = " << fileName << std::endl;
				// TODO stat might not work for symbolic links
				// update_metadata_in_memory(mainHeader, fileName, inputList[i], metaVector, archivePtr, flag);
				continue; // continue iteration without calling iterate_through_dir if it is a file
			}
			else {                                  // pipes, sockets or other file types
				mainHeader.otherFileCount++;
				if (DEBUG) std::cout << "DEBUG Name of other file = " << fileName << std::endl;
				append_to_metadata(fileName, inputList[i], metaVector, mainHeader, archivePtr, flag);
				continue; // continue iteration without calling iterate_through_dir if it is a file
			}
		}
		else {
			std::cerr << "ERROR: in opening stat " << strerror(errno) << '\n';
		}
		iterate_through_dir(inputList[i], archivePtr, mainHeader, metaVector, flag);
	}
	write_header_to_disk(mainHeader, archivePtr);
	write_metadata_to_disk(mainHeader, archivePtr, metaVector);
	return 0;
}

/* Recursive iteration through directories in disk and add the information about the file/directory to
    the metadata vector and Immediately write to archive file on disk if its a file */

// TODO Softlinks and Hardlinsk
void iterate_through_dir(std::string &baseDirName,
                         std::fstream &archivePtr,
                         struct Header &mainHeader,
                         std::vector <struct Metadata> &metaVector,
                         char flag) {
	DIR* dirPtr;                   // for opening a directory stream

	/* if there is an error in opening the dir stream */
	dirPtr = opendir(baseDirName.c_str());
	if (dirPtr == nullptr) {
		std::cerr << "ERROR <" << strerror(errno) <<  "> Unable to open " << baseDirName << std::endl;
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

				switch(dirReadPointer->d_type) {
				case DT_REG:// if dirReadPointer is a file
					mainHeader.fileCount++;
					if (DEBUG) std::cout << "DEBUG Name of file = " << fileName << std::endl;
					append_to_metadata(fileName, pathToObject, metaVector, mainHeader, archivePtr, flag);
					break;
				case DT_LNK:// if dirReadPointer is a symbolic link
					mainHeader.symboliclinkCount++;
					if (DEBUG) std::cout << "DEBUG Name of symbolic link = " << fileName << std::endl;
					append_to_metadata(fileName, pathToObject, metaVector, mainHeader, archivePtr, flag);
					break;
				case DT_DIR:// if dirReadPointer is a directory
					mainHeader.directoryCount++;
					if (DEBUG) std::cout << "DEBUG Name of directory = " << fileName<< std::endl;
					update_metadata_in_memory(mainHeader, fileName, pathToObject, metaVector, archivePtr, flag);
					iterate_through_dir(pathToObject, archivePtr, mainHeader, metaVector, flag);
					break;
				default: // other file types sockets or pipes
					mainHeader.otherFileCount++;
					if (DEBUG) std::cout << "DEBUG Name of other file = " << fileName << std::endl;
					append_to_metadata(fileName, pathToObject, metaVector, mainHeader, archivePtr, flag);
					break;
				}
			}
			dirReadPointer = readdir(dirPtr);
		}
		closedir(dirPtr);
	}
}

/* Write header to disk */
int write_header_to_disk(struct Header &mainHeader,
                         std::fstream &archivePtr) {
	if (archivePtr.is_open()) {
		archivePtr.clear();                 // clear the eof flag if it has been set for archivePtr
		archivePtr.seekp(0, std::ios::beg); // reset the archivePtr to the start of the archive file
		int mainHeaderSize = sizeof(mainHeader);
		archivePtr.write(reinterpret_cast<char*>(&mainHeader), mainHeaderSize);
	}
	else {
		std::cerr << "Failed to open archive file for writing header info" << std::endl;
		return -1;
	}
	return 0;
}

// TODO TODO TODO TODO TODO TODO TODO
/* Read netadata from the archive file */
int read_metadata_from_disk(std::fstream &archivePtr, struct Header &mainHeader, std::vector<struct Metadata> &metaVector){
	archivePtr.seekg(mainHeader.offsetToMeta);    // Seek to the offset of metadata Header.offsetToMeta
  for(int i = 0; i < mainHeader.fileCount + mainHeader.directoryCount; i++){
    struct Metadata meta;
    archivePtr.read((char *)&meta, sizeof(Metadata));
    metaVector.push_back(meta);
  }
	return 0;
}

//  TODO TODO TODO TODO TODO TODO
/* Write our medata struct from from the archive file */
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
                                       std::string &fileName,
                                       std::string &pathToObject) {
	// Getting the information about the file/directory
	struct stat fileStat;
	if (stat((pathToObject).c_str(), &fileStat) == -1) {
		std::cerr << "ERROR <" << errno << "> Unable to get stat info on " << pathToObject << '\n';
	}

	std::string userID, groupID, mode,
	            size, accessDate, modifyDate,
	            changeDate, birthDate, inode;
	nlink_t numLinks;

	userID = std::to_string(fileStat.st_uid);
	groupID = std::to_string(fileStat.st_gid);
	mode = std::to_string(fileStat.st_mode);
	size = std::to_string(fileStat.st_size);
	accessDate = std::to_string(fileStat.st_atime);
	modifyDate = std::to_string(fileStat.st_mtime);
	changeDate = std::to_string(fileStat.st_ctime);
	birthDate = std::to_string(fileStat.st_birthtime);
	inode = std::to_string(fileStat.st_ino);
	numLinks = fileStat.st_nlink;

	if (DEBUG==0) {
		std::cout << "DEBUG File name: " << pathToObject << std::endl;
		std::cout << "    DEBUG User ID: " << fileStat.st_uid << std::endl;
		std::cout << "    DEBUG Group ID: " << fileStat.st_gid << std::endl;
		std::cout << "    DEBUG Mode: " << fileStat.st_mode << std::endl;
		std::cout << "    DEBUG Size: " << fileStat.st_size << std::endl;
		std::cout << "    DEBUG Number of links to the object: " << fileStat.st_nlink << std::endl;
		std::cout << "    DEBUG Inode number: " << fileStat.st_ino << std::endl;
		std::cout << "    DEBUG Last access date: " << fileStat.st_atime << std::endl;
		std::cout << "    DEBUG Last modification date: " << fileStat.st_mtime << std::endl;
		std::cout << "    DEBUG Last change date: " << fileStat.st_ctime << std::endl;
		std::cout << "    DEBUG Birth / creation date: " << fileStat.st_birthtime << std::endl;
	}

	Metadata currentMeta;
	// if the object in question is a directory
	if (S_ISDIR(fileStat.st_mode)) {
		currentMeta.directory = 1;
		currentMeta.file = 0;
		currentMeta.offsetToFileStart = -1; // -1 for directories
		currentMeta.softlink = 0;
	}
	// additional checks for symbolic links
	else if (S_ISLNK(fileStat.st_mode)) {
		currentMeta.directory = 0;
		currentMeta.file = 0;
		currentMeta.offsetToFileStart = -1; // -1 for softlinks
		currentMeta.softlink = 1;
	}
	// then it is a file (Maybe not a regular file)
	else {
		currentMeta.file = 1;
		currentMeta.directory = 0;
		currentMeta.offsetToFileStart = mainHeader.offsetToMeta;
		currentMeta.softlink = 0;
	}
	// As sizeof(string) = size of pointer to string, we have to use string.length()
	currentMeta.version = 1; // Can be updated when the -a flag is used
	currentMeta.numberOfLinks = fileStat.st_nlink;
	snprintf(currentMeta.pathToObject, sizeof(currentMeta.pathToObject), "%s",  (pathToObject).c_str());
	snprintf(currentMeta.fileName, sizeof(currentMeta.fileName), "%s", (fileName).c_str());

	currentMeta.inode = fileStat.st_ino;
	currentMeta.fileSize = fileStat.st_size;
	currentMeta.userID = fileStat.st_uid;
	currentMeta.groupID = fileStat.st_gid;
	currentMeta.filePermission = fileStat.st_mode;
	currentMeta.birthDate = fileStat.st_birthtime;
	currentMeta.accessDate = fileStat.st_atime;
	currentMeta.modifyDate = fileStat.st_mtime;
	currentMeta.changeDate = fileStat.st_ctime;
	/* Security present for string copy
	   snprintf(currentMeta.inode, sizeof(currentMeta.inode), "%s", (inode).c_str());
	   snprintf(currentMeta.fileSize, sizeof(currentMeta.fileSize), "%s", (size).c_str());
	   snprintf(currentMeta.userID, sizeof(currentMeta.userID), "%s", (userID).c_str());
	   snprintf(currentMeta.groupID, sizeof(currentMeta.groupID), "%s", (groupID).c_str());
	   snprintf(currentMeta.filePermission, sizeof(currentMeta.filePermission), "%s", (mode).c_str());
	   snprintf(currentMeta.birthDate,  sizeof(currentMeta.birthDate), "%s", (birthDate).c_str());
	   snprintf(currentMeta.accessDate, sizeof(currentMeta.accessDate), "%s", (accessDate).c_str());
	   snprintf(currentMeta.modifyDate, sizeof(currentMeta.modifyDate), "%s", (modifyDate).c_str());
	   snprintf(currentMeta.changeDate, sizeof(currentMeta.changeDate), "%s", (changeDate).c_str());
	 */
	/* Security not present for string copy
	   strncpy(currentMeta.pathToObject, (pathToObject+"\0").c_str(), pathToObject.length()+1)
	   strncpy(currentMeta.inode, (inode+"\0").c_str(), inode.length()+1);
	   strncpy(currentMeta.fileSize, (size+"\0").c_str(), size.length()+1);
	   strncpy(currentMeta.userID, (userID+"\0").c_str(), userID.length()+1);
	   strncpy(currentMeta.groupID, (groupID+"\0").c_str(), groupID.length()+1);
	   strncpy(currentMeta.filePermission, (mode+"\0").c_str(), mode.length()+1);
	   strncpy(currentMeta.fileName, (fileName+"\0").c_str(), fileName.length()+1);
	   strncpy(currentMeta.birthDate, (birthDate+"\0").c_str(), birthDate.length()+1);
	   strncpy(currentMeta.accessDate, (accessDate+"\0").c_str(), accessDate.length()+1);
	   strncpy(currentMeta.modifyDate, (modifyDate+"\0").c_str(), modifyDate.length()+1);
	   strncpy(currentMeta.changeDate, (changeDate+"\0").c_str(), changeDate.length()+1);
	 */
	return currentMeta; // return the populated Metadata object
}

/* Add updated version or new Meta struct to Metadata in memory*/
/* When using -a append flag, search through Global Metadata Struct to see if a prev ver exists
   Push an updated version meta to vector if found
   Push a new meta to vector if not found
   For flag = c, push to metaVector directly without checking for old versions */
int update_metadata_in_memory( struct Header &mainHeader,
                               std::string &fileName,
                               std::string &pathToObject,
                               std::vector <struct Metadata> &metaVector,
                               std::fstream &archivePtr,
                               char flag) {
	/* create and populate a new Metadata object */
	Metadata currentMeta;
	currentMeta = create_Metadata_object(mainHeader, fileName, pathToObject);
	/* if flag is 'a' then search through the Global Metadata Struct to update the version
	   if not found, push to vector */
	if (flag == 'a') {
		bool prevMetaFound = false;
		// IMPORTANT MORE CHECKS REQUIRED HERE
		for(std::vector<int>::size_type i = 0; i != metaVector.size(); i++) {
			/* if an older version of the file has been found, add a new version of object to metadata */
			if ((std::string)(metaVector[i].pathToObject) == pathToObject) {
				currentMeta.version += 1; // increment the version number
				metaVector.push_back(currentMeta);
				prevMetaFound = true;
				break;
			}
		}
		// if no older version of the metadata was found, add without incrementing version
		if (!prevMetaFound) metaVector.push_back(currentMeta);
	}
	/* if flag is 'c' add to vector directly */
	else if (flag == 'c') {
		metaVector.push_back(currentMeta);
	}
	return 0;
}

/* Append to Metadata in memory and Add New Files to archive file on disk
   This function should only be called for files not directories */
int append_to_metadata(std::string &fileName,
                       std::string &pathToObject,
                       std::vector <struct Metadata> &metaVector,
                       struct Header &mainHeader,
                       std::fstream &archivePtr,
                       char flag){
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
                        std::string &pathToObject,
                        struct Header &mainHeader
                        ) {
	/* appends file to current pffsetToMeta */
	std::ifstream readFile;  // open a input file stream
	readFile.open(pathToObject, std::ios::binary);
	if (DEBUG==0) std::cout << "DEBUG 1 Current FILE writer ptr loc in archivePtr is " << archivePtr.tellp() << std::endl;
	archivePtr.clear();
	archivePtr.seekp(mainHeader.offsetToMeta); // set pointer to the start of the offsetToMeta
	if (DEBUG==0) std::cout << "DEBUG   2 Current FILE writer ptr loc in archivePtr is " << archivePtr.tellp() << std::endl;

  /* Write the content to the archive */
	if (readFile.is_open()) {
		char buffer;
		while (!readFile.eof()) {
			buffer = (char) readFile.get();
			archivePtr.put(buffer);
			// Update the offsetToMeta offset
			mainHeader.offsetToMeta += sizeof(buffer);
			// std::cout << "APtr = " << archivePtr.tellg() << '\n';
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

/* convert UNIX time format to date format */
char * unix_time_to_date (time_t unixTime) {
	struct tm *tm = localtime(&unixTime);
	static char convertedDate[20];
	strftime(convertedDate, sizeof(convertedDate), "%Y-%m-%d %H:%M", tm);
	return (char *)convertedDate;
}

/* converting mode_t to a permission string in format -rwxrwxrwx*/
char * mode_to_permission (mode_t fileMode) {
	static char convertedFilePermission[11];
	if (S_ISLNK(fileMode)) { // checking if the object is a symbolic link
		convertedFilePermission[0] = 'l';
	}
	else {
		// set convertedFilePermission[0] to 'd' if directory
		convertedFilePermission[0] = ((S_ISDIR(fileMode)) ? 'd' : '-');
	}
	convertedFilePermission[1] = (fileMode & S_IRUSR) ? 'r' : '-';
	convertedFilePermission[2] = (fileMode & S_IWUSR) ? 'w' : '-';
	convertedFilePermission[3] = (fileMode & S_IXUSR) ? 'x' : '-';
	convertedFilePermission[4] = (fileMode & S_IRGRP) ? 'r' : '-';
	convertedFilePermission[5] = (fileMode & S_IWGRP) ? 'w' : '-';
	convertedFilePermission[6] = (fileMode & S_IXGRP) ? 'x' : '-';
	convertedFilePermission[7] = (fileMode & S_IROTH) ? 'r' : '-';
	convertedFilePermission[8] = (fileMode & S_IWOTH) ? 'w' : '-';
	convertedFilePermission[9] = (fileMode & S_IXOTH) ? 'x' : '-';
	convertedFilePermission[10] = '\0';
	return convertedFilePermission;
}

/* converts uid to user name */
std::string get_user_name (uid_t uid) {
	struct passwd *pswrd;
	pswrd = getpwuid(uid);
	return pswrd->pw_name;
}

/* convertes gid to group id */
std::string get_group_name (uid_t gid) {
	struct group *grp;
	grp = getgrgid(gid);
	return grp->gr_name;
}

/*  Extracts file name from pathToObject
    Utility code from
   C++ Cookbook by Jeff Cogswell, Jonathan Turkanis, Christopher Diggins, D. Ryan Stephens */
std::string get_filename_from_path(std::string &pathToObject) {
	char sep = '/';
	#ifdef _WIN32 // for windows
	sep = '\\';
	#endif

	size_t i = pathToObject.rfind(sep, pathToObject.length());
	if (i != std::string::npos) {
		return(pathToObject.substr(i+1, pathToObject.length() - i));
	}
	return(pathToObject); // if no / or \\ were found, return original name
}
