#include "extract.h"

/* Implementation for -x flag*/

/* Extracts version, or if -1, extracts the newest version only */
//TODO: Implement -1
extract_archive_version(std::fstream &archivePtr, std::vector<struct Metadata> &metaVector, int version){
  for(std::vector<struct Metadata> it = metaVector.begin(); it != metaVector.end(); ++it) {
    if (*it.version == version){ //This is the correct version
      extract_archive(archivePtr, *it);
    }
  }
}

/* Extracts one file or directory */
extract_archive(std::fstream &archivePtr, struct Metadata &meta){
  //If this is a directory we don't need to copy file.
  if (meta.directory == 1){
    fopen (meta.pathToObject,"a");
  } else {
    //TODO: Copy all bytes of archive to the output directory
    pFile = fopen (meta.pathToObject,"w");
    archivePtr.write()
  }
}
