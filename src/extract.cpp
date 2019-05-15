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
int extract_archive_version(std::fstream &archivePtr, std::vector<struct Metadata> &metaVector, int version){
  if (DEBUG) std::cout << "ready to extract_archive_version  " << version << std::endl;
  std::vector<struct Metadata>filtered;
  if (version == -1){
    filtered = create_filtered_latest_metadata(metaVector);
  } else {
    filtered = create_filtered_version_metadata(metaVector, version);
  }

  //Extract all the required files
	for(std::vector<struct Metadata>::iterator it = filtered.begin(); it != filtered.end(); ++it) {
		extract_meta_file(archivePtr, *it);
	}
	return(0);
}


/* Filters metaVector and returns a filtered medatada with only the latest version of each meta */
/* Algorithm for filtering:
1.Make a copy of metaVector and sort on version
2.Loop over the sorted copy and append to filtered vector
3. If filtered vector already contains a metadata with the same file, skip. */
std::vector<struct Metadata> create_filtered_latest_metadata(std::vector<struct Metadata> &metaVector){
  //The new filtered vector we will be returned
  std::vector<struct Metadata> filtered;
  return(filtered);
}

/* Filters metaVector and returns a filtered medatada with only specified version */
std::vector<struct Metadata> create_filtered_version_metadata(std::vector<struct Metadata> &metaVector, int version){
  //The new filtered vector returned
  std::vector<struct Metadata> filtered;
  //Get only metadata with our version
  for(std::vector<struct Metadata>::iterator it = metaVector.begin(); it != metaVector.end(); ++it) {
		if (it->version == version) { //This is the correct version
		filtered.push_back(*it);
		}
	}
  return(filtered);
}

/* Extracts one file or directory */
int extract_meta_file(std::fstream &archivePtr, struct Metadata &meta){
	char updated_pathToObject[FILENAME_MAX+1];
	strcpy(updated_pathToObject, "test");
	strcat(updated_pathToObject, meta.pathToObject);

	if (DEBUG) std::cout << "Extracting " << updated_pathToObject << "  version " << meta.version << '\n';
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
