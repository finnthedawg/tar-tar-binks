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
    if (it->version == version){ //This is the correct version
      extract_archive(archivePtr, *it);
    }
  }
  return(1);
}

/* Extracts one file or directory */
int extract_archive(std::fstream &archivePtr, struct Metadata &meta){
  char buf[1024] = ""; //Buffer used when reading from archive.
  //If this is a directory we don't need to copy file.
  if (meta.directory == 1){
    mkdir(meta.pathToObject,meta.filePermission);
    return(1);
  } else {
    //TODO: Copy all bytes of archive to the output directory
    std::fstream diskPtr;
  	diskPtr.imbue(std::locale::classic());
  	diskPtr.open(meta.pathToObject, std::ios::app | std::ios::binary);
    archivePtr.seekg(meta.offsetToFileStart ,archivePtr.beg); //Seek to the correct position in
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
    } while ( i < meta.fileSize);
    diskPtr.close();
  }
  return(1);
}
