#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "meta.h"
#include "common.h"

/* Implementation for -m flag*/
int display_metadata_from_archive (std::vector <struct Metadata> &metaVector, std::string &pathToObject, bool displayAll) {
	/* If we want to display metadata of all objects */
	if (displayAll) {
		std::cout << "Printing the metadata of all objects from the archive" << '\n';
		for(std::vector<int>::size_type i = 0; i != metaVector.size(); i++) {
			display_one_metadata_attrb(metaVector[i]);
		}
		return 0;
	}
	/* if we want to display metatdata of selected objects by path to the objects */
	else  {
		std::cout << "Printing the metadata of " << pathToObject << " from the archive" << '\n';
		for(std::vector<int>::size_type i = 0; i != metaVector.size(); i++) {
			if (metaVector[i].pathToObject == pathToObject) {
				display_one_metadata_attrb(metaVector[i]);
				return 0;
			}
		}
		std::cerr << "Object referenced by path" << pathToObject << " not found in archive" << '\n';
	}
	return -1;
}

/* display all attributes on Metadata object */
void display_one_metadata_attrb (struct Metadata &metadata_object) {
	if (metadata_object.file) std::cout << "Type: File" << '\n';
	if (metadata_object.directory) std::cout << "Type: Directory" << '\n';
	if (metadata_object.softlink) std::cout << "Type: Softlink" << '\n';
	if (!(metadata_object.file || metadata_object.directory || metadata_object.softlink)) {
		std::cout << "Type: Other file type" << '\n';
	}

	std::cout << "  " << "name: " << metadata_object.fileName << std::endl;
	std::cout << "  " << "path to object: " << metadata_object.pathToObject << std::endl;
	if (metadata_object.softlink) std::cout << "  " << "Symbolic link target path: " << metadata_object.symLinkTarget << std::endl;
	std::cout << "  " << "inode: " << metadata_object.inode << std::endl;

	std::cout << "  " << "permission " << mode_to_permission(metadata_object.filePermission) << std::endl;
	// std::cout << "  " << "permission " << metadata_object.filePermission << std::endl;

	// std::cout << "  " << "uid: " << get_user_name(metadata_object.userID) << std::endl;
	// std::cout << "  " << "gid: " << get_group_name(metadata_object.groupID) << std::endl;
	std::cout << "  " << "uid: " << metadata_object.userID << std::endl;
	std::cout << "  " << "gid: " << metadata_object.groupID << std::endl;

	std::cout << "  " << "version: " << metadata_object.version << std::endl;
	std::cout << "  " << "size: " << metadata_object.fileSize << std::endl;

	std::cout << "  " << "creation time: " << unix_time_to_date(metadata_object.birthDate) << std::endl;
	std::cout << "  " << "access time: " << unix_time_to_date(metadata_object.accessDate) << std::endl;
	std::cout << "  " << "modify time: " << unix_time_to_date(metadata_object.modifyDate) << std::endl;
	std::cout << "  " << "change time: " << unix_time_to_date(metadata_object.changeDate) << std::endl;
	// std::cout << "  " << "birthtime: " << metadata_object.birthDate << std::endl;
	// std::cout << "  " << "accesstime: " << metadata_object.accessDate << std::endl;
	// std::cout << "  " << "modifytime: " << metadata_object.modifyDate << std::endl;
	// std::cout << "  " << "changetime: " << metadata_object.changeDate << std::endl;

	std::cout << "  " << "number of links: " << metadata_object.numberOfLinks << std::endl;
}
