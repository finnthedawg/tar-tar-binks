#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include "common.h"

/* Recursive iteration through directories in disk and add the information about the file/directory to
    the metadata vector and Immediately write to archive file on disk if its a file */

// TODO Softlinks and Hardlinsk
void iterate_through_dir(std::string baseDirName, std::fstream& archivePtr, struct Header mainHeader) {
	DIR* dirPtr;                   // for opening a directory stream
	struct dirent *dirReadPointer; // pointer for reading the DIR stream

	/* if there is an error in opening the dir stream */
	dirPtr = opendir(baseDirName.c_str());
	if (dirPtr == nullptr) {
		std::cerr << "ERROR <" << errno <<  "> Unable to open " << baseDirName << std::endl;
	}
	else {
		std::cout << "DEBUG " << "Opening " << baseDirName << std::endl;
		dirReadPointer = readdir(dirPtr);

		while (dirReadPointer) {
			/* excluding current dir and parent directory specifier . and .. along with DS_Store */
			if ( dirReadPointer->d_name != std::string(".") && dirReadPointer->d_name != std::string("..") &&
			     dirReadPointer->d_name != std::string(".DS_Store") ) {
				// Getting the information about the file/directory
				struct stat fileStat;
				if (stat((baseDirName+"/"+dirReadPointer->d_name).c_str(), &fileStat) == -1) {
					std::cerr << "ERROR <" << errno << "> Unable to get stat info on " << baseDirName+"/"+dirReadPointer->d_name << '\n';
				}

				std::string uid, gid, mode, size, modifyDate, inode;
				int numHardLinks;

				uid = std::to_string(fileStat.st_uid);
				gid = std::to_string(fileStat.st_gid);
				mode = std::to_string(fileStat.st_mode);
				size = std::to_string(fileStat.st_size);
				modifyDate = std::to_string(fileStat.st_mtime);
				inode = std::to_string(fileStat.st_ino);
				numHardLinks = (int)fileStat.st_nlink;


				std::cout << "File name: " << baseDirName << "/" << dirReadPointer->d_name  << std::endl;
				std::cout << "User ID: " << fileStat.st_uid << std::endl;
				std::cout << "Group ID: " << fileStat.st_gid << std::endl;
				std::cout << "Mode: " << fileStat.st_mode << std::endl;
				std::cout << "Size: " << fileStat.st_size << std::endl;
				std::cout << "Number of hard links to the file: " << fileStat.st_nlink << std::endl;
				std::cout << "Last modification date: " << fileStat.st_mtime << std::endl;
				std::cout << "Inode number: " << fileStat.st_ino << std::endl;

				if (dirReadPointer->d_type == DT_REG) {  // if dirReadPointer is a file
					mainHeader.fileCount++;
					std::cout << dirReadPointer->d_name << std::endl;

					// TODO
					// append_to_metadata(vector <struct Metadata>* metaVector, read_file_pointer)
				}
				else if (dirReadPointer->d_type == DT_DIR) { // if dirReadPointer is a directory
					mainHeader.directoryCount++;
					std::cout << dirReadPointer->d_name << std::endl;

					// TODO
					// update_metadata_in_memory(vector <struct Metadata> *metaVector, read_file_pointer)
					iterate_through_dir(baseDirName+"/"+dirReadPointer->d_name, archivePtr, mainHeader);
				}
			}
			dirReadPointer = readdir(dirPtr);
		}
		closedir(dirPtr);
	}
}
