#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "extract.h"
#include "common.h"

/* Implementation for -x flag*/

/* Extracts version, or if -1, extracts the newest version only */
//TODO: Implement -1
int extract_archive_version(std::fstream &archivePtr, std::vector<struct Metadata> &metaVector, int version){
	for(std::vector<struct Metadata>::iterator it = metaVector.begin(); it != metaVector.end(); ++it) {
		if (it->version == version) { //This is the correct version
			extract_meta_file(archivePtr, *it);
		}
	}
	return(0);
}


/* Filters metaVector and returns a filtered medatada with only the latest version of each meta */
std::vector<struct Metadata> create_filtered_latest_metadata(std::vector<struct Metadata> &metaVector){
  //The new filtered vector we will be returned
  std::vector<struct Metadata> filtered;
  for(std::vector<struct Metadata>::it = metaVector.begin(); it != metaVector.end(); it++){
    filtered.push_back(*it);
  }
  //Begin the filter process. Each time
  return(filtered);
}

/* Filters metaVector and returns a filtered medatada with only specified version */
std::vector<struct Metadata> create_filtered_version_metadata(std::vector<struct Metadata> &metaVector, int version){
  //The new filtered vector we will be returned
  std::vector<struct Metadata> filtered;
  for(std::vector<struct Metadata>::it = metaVector.begin(); it != metaVector.end(); it++){
    filtered.push_back(*it);
  }
  //Begin the filter process. Each time
  return(filtered);
}

/* Extracts one file or directory */
int extract_meta_file(std::fstream &archivePtr, struct Metadata &meta){
	char updated_pathToObject[FILENAME_MAX+1];
	strcpy(updated_pathToObject, "test");
	strcat(updated_pathToObject, meta.pathToObject);

	if (DEBUG) std::cout << "Ready to extract " << updated_pathToObject << '\n';
	char buf[1024] = ""; //Buffer used when reading from archive.
	// If this is a directory we don't need to copy file.
	if (meta.directory == 1) {
		mkdir(updated_pathToObject,meta.filePermission);
		return(0);
	}
	else {
		// Copy all data of archive to the output directory
		std::fstream diskPtr;
		diskPtr.imbue(std::locale::classic());
		diskPtr.open(updated_pathToObject, std::ios::app | std::ios::binary);
		archivePtr.seekg(meta.offsetToFileStart, archivePtr.beg); //Seek to the correct position in
		int readAmount = 0; int i=0;
		do {
			i += 1024;
			//If I exceeds file size, then i is the file size.
			if (i > meta.fileSize) {
				i = meta.fileSize;
			}
			readAmount = i%1025;
			archivePtr.read(buf,readAmount);
			diskPtr.write(buf, readAmount);
		} while (i < meta.fileSize);
		diskPtr.close();
		chmod(updated_pathToObject, meta.filePermission);
	}
	return(0);
}
