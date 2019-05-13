#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "common.h"
#include "hierarchy.h"

/* Implementation for -p flag*/
/* display heirarchy of all objects in metaVector */
void display_hierarchy_from_archive (std::vector <struct Metadata> metaVector) {
    for(std::vector<int>::size_type i = 0; i != metaVector.size(); i++) {
        std::cout << metaVector[i].pathToObject << '\n';
    }
}
