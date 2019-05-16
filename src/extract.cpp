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
  //The copy we will sort and check.
  std::vector<struct Metadata> copy;
  for(std::vector<struct Metadata>::iterator it = metaVector.begin(); it != metaVector.end(); it++){
    copy.push_back(*it);
  }
  //Sorting
  std::sort(copy.begin(), copy.end(), [](struct Metadata meta1, struct Metadata meta2){
    return meta1.version > meta2.version;
  });
  //Checking if it already exists
  for(std::vector<struct Metadata>::iterator it = copy.begin(); it != copy.end(); it++){
    //See if this exists in output filtered vector
    std::vector<struct Metadata>::iterator out_it = std::find_if(filtered.begin(), filtered.end(), [](struct Metadata meta){
      std::string filtered(meta.pathToObject); //In output results
      std::string sorted(meta.pathToObject); //Searching for this
      std::cout << "filtered " << filtered << std::endl;
      std::cout << "filtered " << sorted << std::endl;
      if (filtered.compare(sorted) == 0){
        return(1); // meta found
      } else {
        return(0); //meta not found
      }
    });
    //If doesn't, then add it to output filtered vector
    if (out_it != filtered.end()){
      filtered.push_back(*it);
    }
  }
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
