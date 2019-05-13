#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include "store.h"
#include "common.h"

/* Implementation for -c flag*/
/* function to create an archive file */
int store_archive(std::vector<std::string> inputList, std::fstream &archivePtr,
                  struct Header &mainHeader, std::vector<struct Metadata>& metaVector) {


	// iterating through the inputList vector to get all the files/dirs to archive
	// iterate_through_dir write the contents of all files that have been found
	// starting from the mainHeader.offsetToMeta
	for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
		if (DEBUG) std::cout << "DEBUG " << inputList[i] << std::endl;
		iterate_through_dir(inputList[i], archivePtr, mainHeader, metaVector, 'c');
	}
	write_header_to_disk(mainHeader, archivePtr);
	write_metadata_to_disk(mainHeader, archivePtr, metaVector);
	return -1;
}
