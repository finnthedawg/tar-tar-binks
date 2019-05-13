#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "meta.h"
#include "common.h"

/* Implementation for -m flag*/
void display_metadata_from_archive (std::vector <struct Metadata> metaVector) {
    for(std::vector<int>::size_type i = 0; i != metaVector.size(); i++) {

        std::cout << metaVector[i].fileName << '\n';
    }
}
