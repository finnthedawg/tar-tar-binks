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
		return (-1*path1.compare(path2));
		if (path1.compare(path2) == 0){
			return ((int)(meta1.version < meta2.version));
		} else {
			return (-1*path1.compare(path2));
		}
	});
	/* Print the hierarchy along with the version */
	for(std::vector<int>::size_type i = 0; i != metaVector.size(); i++) {
		std::cout << metaVector[i].pathToObject << " version: " <<  metaVector[i].version << '\n';
	}
}
