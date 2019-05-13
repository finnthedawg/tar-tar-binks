#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include "append.h"
#include "common.h"

/* Implementation for -a flag*/
/* function to append to the archive file */
int append_archive (std::vector<std::string> inputList, std::fstream &archivePtr,
                    struct Header &mainHeader, std::vector<struct Metadata>& metaVector, char flag) {

	add_objects_to_archive(inputList, archivePtr, mainHeader, metaVector, 'a');
	return 0;
}
