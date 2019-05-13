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
                  struct Header &mainHeader, std::vector<struct Metadata>& metaVector, char flag) {

	add_objects_to_archive(inputList, archivePtr, mainHeader, metaVector, 'c');
	return 0;
}
