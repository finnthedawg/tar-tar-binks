#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h> // For symlink
#include "extract.h"
#include "common.h"

/* Implementation for -x flag*/

/* Extracts version, or if -1, extracts the newest version only */
int extract_archive_version(std::fstream &archivePtr, std::vector<struct Metadata> &metaVector, int version){
	if (DEBUG) std::cout << "ready to extract_archive_version  " << version << std::endl;
	std::vector<struct Metadata>filtered;
	if (version == -1) {
		filtered = create_filtered_latest_metadata(metaVector);
	} else {
		filtered = create_filtered_version_metadata(metaVector, version);
	}

	/* Extract all the required files */
	//First go through and extract all files and hard linkes
	for(std::vector<struct Metadata>::iterator it = filtered.begin(); it != filtered.end(); ++it) {
		if(it->softlink == 0) {
			extract_meta_file(archivePtr, *it);
		}
	}

	//Then extract all soft links
	for(std::vector<struct Metadata>::iterator it = filtered.begin(); it != filtered.end(); ++it) {
		if(it->softlink == 1) {
			extract_meta_softlink(archivePtr, *it);
		}
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
	//The copy we will sort and check.
	std::vector<struct Metadata> copy;
	for(std::vector<struct Metadata>::iterator it = metaVector.begin(); it != metaVector.end(); it++) {
		copy.push_back(*it);
	}
	//if (DEBUG) std::cout << "DEBUG There are this many files and dirs: " << copy.size() << '\n';
	if (DEBUG) std::cout << "DEBUG filter done copying " << '\n';
	//Sorting
	std::sort(copy.begin(), copy.end(), [](struct Metadata meta1, struct Metadata meta2){
		return meta1.version > meta2.version;
	});
	if (DEBUG) std::cout << "DEBUG filter function done sorting " << '\n';
	//Checking if it already exists
	for(std::vector<struct Metadata>::iterator it = copy.begin(); it != copy.end(); it++) {
		if (DEBUG) std::cout << "DEBUG filter checking " << (*it).pathToObject << '\n';
		//See if this exists in output filtered vector
		std::vector<struct Metadata>::iterator out_it = std::find_if(filtered.begin(), filtered.end(), [=](struct Metadata meta){
			std::string filtered(meta.pathToObject); //In output results
			std::string sorted(it->pathToObject); //Searching for this
			if (filtered.compare(sorted) == 0) {
			    return(1); // meta found
			} else {
			    return(0); //meta not found
			}
		});
		//If doesn't, then add it to output filtered vector
		if (out_it == filtered.end()) {
			if (DEBUG) std::cout << "Doesnt exist, pushing back " << (*it).pathToObject << '\n';
			filtered.push_back(*it);
		}
	}
	if (DEBUG) std::cout << "DEBUG filter done filtering " << '\n';
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
	//char buf[1024] = ""; //Buffer used when reading from archive.
	// If this is a directory we don't need to copy file.
	if (meta.directory == 1) {
		mkdir(updated_pathToObject,meta.filePermission);
		chmod(updated_pathToObject,meta.filePermission);
		return(0);
	}
	else {
		// Copy all data of archive to the output directory
		std::fstream diskPtr;
		std::string pathonly(updated_pathToObject);
		pathonly = get_pathonly_from_path(pathonly);
		const char * pathonly_const = pathonly.c_str();
		std::cout << "found file, creating the directory" << pathonly_const << std::endl;
		mkdir(pathonly_const,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //If the directory doest exist, create it with temp permissions.
		diskPtr.imbue(std::locale::classic());
		diskPtr.open(updated_pathToObject, std::ios::app | std::ios::binary);
		archivePtr.seekg(meta.offsetToFileStart, archivePtr.beg); //Seek to the correct position in
		// Old method of reading to a buffer
		// int readAmount = 0; int i=0;
		// do {
		//  i += 1024;
		//  //If I exceeds file size, then i is the file size.
		//  if (i > meta.fileSize) {
		//      i = meta.fileSize;
		//  }
		//  readAmount = i%1025;
		//  archivePtr.read(buf,readAmount);
		//  diskPtr.write(buf, readAmount);
		//   std::cout << buf;
		// } while (i < meta.fileSize);

		for(int i = 0; i < meta.fileSize; i++) {
			char buffer;
			buffer = (char) archivePtr.get();
			diskPtr.put(buffer);
		}

		diskPtr.close();
		chmod(updated_pathToObject, meta.filePermission);
	}
	return(0);
}

/* Extracts one file or directory */
int extract_meta_softlink(std::fstream &archivePtr, struct Metadata &meta){
	char updated_pathToObject[FILENAME_MAX+1];
	strcpy(updated_pathToObject, "test");
	strcat(updated_pathToObject, meta.pathToObject);

	char updated_pathTosymlink[FILENAME_MAX+1];
	strcpy(updated_pathTosymlink, "test");
	strcat(updated_pathTosymlink, meta.symLinkTarget);

	if (DEBUG) std::cout << "Extracting file with symbolic link " << updated_pathToObject << "  version " << meta.version << '\n';
	//char buf[1024] = ""; //Buffer used when reading from archive.
	// If this is a directory we don't need to copy file.
	symlink(updated_pathTosymlink, updated_pathToObject);

	// if (meta.directory == 1) {
	//   symlink(const char  *name1, meta.updated_pathToObject);
	//  return(0);
	// } else {
	//  // Copy all data of archive to the output directory
	//  std::fstream diskPtr;
	//  std::string pathonly(updated_pathToObject);
	//  pathonly = get_pathonly_from_path(pathonly);
	//  const char * pathonly_const = pathonly.c_str();
	//  std::cout << "found file, creating the directory" << pathonly_const << std::endl;
	//  mkdir(pathonly_const,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //If the directory doest exist, create it with temp permissions.
	//  diskPtr.imbue(std::locale::classic());
	//  diskPtr.open(updated_pathToObject, std::ios::app | std::ios::binary);
	//  archivePtr.seekg(meta.offsetToFileStart, archivePtr.beg); //Seek to the correct position in
	//   // Old method of reading to a buffer
	//  // int readAmount = 0; int i=0;
	//  // do {
	//  //  i += 1024;
	//  //  //If I exceeds file size, then i is the file size.
	//  //  if (i > meta.fileSize) {
	//  //      i = meta.fileSize;
	//  //  }
	//  //  readAmount = i%1025;
	//  //  archivePtr.read(buf,readAmount);
	//  //  diskPtr.write(buf, readAmount);
	//   //   std::cout << buf;
	//  // } while (i < meta.fileSize);
	//
	//   for(int i = 0; i < meta.fileSize; i++){
	//     char buffer;
	//     buffer = (char) archivePtr.get();
	//     diskPtr.put(buffer);
	//   }
	//
	//  diskPtr.close();
	//  chmod(updated_pathToObject, meta.filePermission);
	// }
	return(0);
}
