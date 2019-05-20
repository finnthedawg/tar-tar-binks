#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <algorithm>
#include <string>
#include "common.h"
#include "hierarchy.h"

/* Implementation for -p flag*/
/* display heirarchy of all objects in metaVector */
void display_hierarchy_from_archive (std::vector <struct Metadata> &metaVector) {
	/* Sort the metadata based on the directory, and then the version */
	sort(metaVector.begin(), metaVector.end(), [](struct Metadata meta1, struct Metadata meta2){
		std::string path1(meta1.pathToObject);
		std::string path2(meta2.pathToObject);
		//std::cout << "Comparing 1 " << path1 << std::endl;
		//std::cout << "Comparing 2 " << path2 << std::endl;
		if (path1.compare(path2) == 0) {
			//std::cout << "Got 0 " << std::endl;
			//std::cout << "version 1 " << meta1.version << std::endl;
			//std::cout << "version 2 " << meta2.version << std::endl;
		    return ((int)(meta1.version < meta2.version));
		} else {
			if(path1.compare(path2) < 0){
				return 1;
			} else {
				return 0;
			}
		   // return (-1*path1.compare(path2));
		}
	});
	/* Print the hierarchy along with the version */
	for(std::vector<int>::size_type i = 0; i != metaVector.size(); i++) {
		std::cout << metaVector[i].pathToObject << " version: " <<  metaVector[i].version << '\n';
	}
}
