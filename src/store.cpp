#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include "store.h"
#include "common.h"
#include "meta.h" // REMOVE LATER

/* Implementation for -c flag*/
/* function to create an archive file */
int store_archive(std::vector<std::string> inputList, std::fstream &archivePtr,
                  struct Header &mainHeader, std::vector<struct Metadata>& metaVector) {
	struct stat initialStat;

	// iterating through the inputList vector to get all the files/dirs to archive
	// iterate_through_dir write the contents of all files that have been found
	// starting from the mainHeader.offsetToMeta
	for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
        std::string fileName = get_filename_from_path(inputList[i]);
        if (DEBUG) std::cout << "DEBUG IMPORTANT Printing file name without leading path " << fileName << '\n';
		if (DEBUG) std::cout << "DEBUG Starting archive with (fullpath) " << inputList[i] << std::endl;

		/* IMPORTANT this if check is only done once in the begining if there is only one main file or folder to archive */
		if (stat((inputList[i]).c_str(), &initialStat) == 0) {
			if (S_ISDIR(initialStat.st_mode)) {     // inputList[i] is a directory
				mainHeader.directoryCount++;
				if (DEBUG) std::cout << "DEBUG: name of directory=" << fileName<< std::endl;
				update_metadata_in_memory(mainHeader, fileName, inputList[i], metaVector, archivePtr, 'c');
			}
			else if(S_ISREG(initialStat.st_mode)) { // inputList[i] is a regular file
				mainHeader.fileCount++;
				if (DEBUG) std::cout << "DEBUG: name of file=" << fileName << std::endl;
				append_to_metadata(fileName, inputList[i], metaVector, mainHeader, archivePtr, 'c');
                continue; // continue iteration without calling iterate_through_dir if it is a file
			}
			else if(S_ISLNK(initialStat.st_mode)) { // inputList[i] is a symbolic link
				mainHeader.symboliclinkCount++;
				if (DEBUG) std::cout << "DEBUG: name of symbolic link = " << fileName << std::endl;
                // TODO stat might not work for symbolic links
                // update_metadata_in_memory(mainHeader, fileName, inputList[i], metaVector, archivePtr, 'c');
                continue; // continue iteration without calling iterate_through_dir if it is a file
			}
			else {                                  // pipes, sockets or other file types
				mainHeader.otherFileCount++;
				if (DEBUG) std::cout << "DEBUG: name of other file = " << fileName << std::endl;
				append_to_metadata(fileName, inputList[i], metaVector, mainHeader, archivePtr, 'c');
                continue; // continue iteration without calling iterate_through_dir if it is a file
			}
		}
		else {
			std::cerr << "ERROR: in opening stat " << strerror(errno) << '\n';
		}
		iterate_through_dir(inputList[i], archivePtr, mainHeader, metaVector, 'c');
	}
	write_header_to_disk(mainHeader, archivePtr);
	write_metadata_to_disk(mainHeader, archivePtr, metaVector);

	if (DEBUG) std::cout << "DEBUG REMOVE LATER PRINTING OUT META INFORMATION FOR TESTING" << '\n';
	display_metadata_from_archive (metaVector, inputList[0], true);
	return 0;
}
