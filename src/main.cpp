/*
 * Function and variable naming convention used
 * all variables = camelCase
 * all function names = function_do_undscr()
 * structs Capitalized names i.e. Metadata, Header
 * All custom defined funcs that have a return value rtn 0 on success and -1 on failure
 */
#include <set>
#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include <csignal>
#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include "meta.h"
#include "store.h"
#include "common.h"
#include "append.h"
#include "extract.h"
#include "hierarchy.h"

/* function to check for valid inputs and store archiveName name in archiveName
    and input files/directory names in inputList string array, tartar flag in flag
    and the version number from -o which will be set to -1 if not specified */
bool check_cmd_args(int argc, char *argv[], std::string& archiveName,
                    std::vector<std::string>& inputList, std::string& flag, int& version);

/* Signal Handler for catching SIGABRT signals */
void signalHandler(int signum);

int main(int argc, char *argv[]) {
	signal(SIGABRT, signalHandler);          // register signal SIGABRT and signal handler
	std::string archiveName, flag;           // name of archive file, flag used
	std::vector<std::string> inputList;      // vector of folder/files to archive/check version or extract
	int version;                             // will be equal to -1 if -o version number is not specified
	Header mainHeader;                       // main Header struct
	std::vector<struct Metadata> metaVector; // vector of Metadata structs

	/* Checking for valid cmd args */
	if (check_cmd_args(argc, argv, archiveName, inputList, flag, version) == false ) {
		// incorrect args error exit
		std::cerr << "Incorrect Usage: ./tartar -flag <archive-file> <file/directory list>" << std::endl;
		return -1;
	}

	/* remove the archive file if it alreday exits for the -c create flag */
	if (flag[1] == 'c') {
		const int result = remove(archiveName.c_str());
		if (DEBUG) {
			if( result == 0 ) {
				std::cout << "DEBUG " << archiveName << " existed and was removed" << std::endl;
			} else {
				std::cout << "DEBUG " << archiveName << " does not exist. No need to remove" << std::endl;
			}
		}
		mainHeader.offsetToMeta = sizeof(mainHeader);   // set the offsetToMeta to size of mainHeader only for -c flag
	}

	std::fstream archivePtr;
	archivePtr.imbue(std::locale::classic());
	archivePtr.open(archiveName, std::ios::app);        // creates file if not present already
	archivePtr.close();
	archivePtr.open(archiveName, std::ios::in |
	                std::ios::out | std::ios::binary ); // open archiveName with rd and wrt perm
	if ( !(archivePtr.is_open()) ) {
		std::cerr << "ERROR: cannot open "<< archiveName << std::endl;
		return -1;
	}

	/* If any other flag than the -c create flag is used. We must read the Metadata from the archive first */
	if (flag[1] != 'c') { //If not creating archive, parse the information
		if (file_size(archivePtr) <= sizeof(mainHeader)) {
			std::cerr << "Empty archive\n";
			exit(-1);
		}
		/* Read the header information */
		archivePtr.read(reinterpret_cast<char*>(&mainHeader),sizeof(mainHeader));
		if (DEBUG) std::cout << "DEBUG Read header offset:" << mainHeader.offsetToMeta \
			                 << " fileCount: " << mainHeader.fileCount << " directoryCount " << mainHeader.directoryCount << std::endl;
	}

	if (DEBUG) std::cout << "DEBUG Archive Name is " <<archiveName << ". Version is "<< version << std::endl;

	// Check the flag type and run the appropriate command
	switch (flag[1]) {
	case 'c': // -c store flag
	{
		if (DEBUG) std::cout << "DEBUG -c flag used" << '\n';
		store_archive(inputList, archivePtr, mainHeader, metaVector, 'c');
		break;
	}
	case 'a': // -a append flag
		if (DEBUG) std::cout << "DEBUG -a flag used" << '\n';
		append_archive(inputList, archivePtr, mainHeader, metaVector, 'a');
		break;
	case 'x': // -x extract flag with/without -o version flag TODO
		if (DEBUG) std::cout << "DEBUG -x flag used" << '\n';
		// iterating through the inputList vector to extract all the correct version
		for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
			if (DEBUG) std::cout << "DEBUG " << inputList[i] << std::endl;
			// TODO
			// run the extract function for each file/dir in the inputList
		}
		break;
	case 'm': // -m print metatdata flag
		if (DEBUG) std::cout << "DEBUG -m flag" << '\n';
		/* If no files are specified then print out Metadata of all files in archive */
		if (argc == 3) {
			display_metadata_from_archive(metaVector, inputList[0], true); // the inputList[0] passed here is bogus
		}
		else {
			// files must be specified with full path i.e. dir1/dir2/dir3/file1.txt not just file1.txt
			// iterating through the inputList vector to print the metadata for all mentioned files/folders
			for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
				if (DEBUG) std::cout << "DEBUG " << inputList[i] << std::endl;
				display_metadata_from_archive(metaVector, inputList[i], false);
			}
		}
		break;
	case 'p': // -p print directory hierarchy flag
		if (DEBUG) std::cout << "DEBUG -p flag." << '\n';
		display_hierarchy_from_archive(metaVector);
		break;
	default:
		std::cout << "ERROR: invalid flags" << '\n';
	}

	// append_archive(inputList, archivePtr, mainHeader, metaVector, 'a');
	if (DEBUG) std::cout << "DEBUG REMOVE LATER PRINTING OUT META INFORMATION FOR TESTING" << '\n';
	// display_metadata_from_archive(metaVector, inputList[0], true);
	std::cout << "DEBUG Main header contents: Offset to meta" << mainHeader.offsetToMeta << " fileCount is " << mainHeader.fileCount << '\n';
	archivePtr.close();
	return 0;
}

/* Signal Handler for catching SIGABRT signals */
void signalHandler(int signum) {
	std::cerr << "Interrupt signal (" << signum << ") received.\n";
	std::cerr << "Correct Usage: ./tartar -flag <archive-file> <file/directory list>" << std::endl;
	// cleanup and close up stuff here
	// terminate program
	exit(signum);
}

/* Function to check for illegal args while invoking the tar archiver */
bool check_cmd_args(int argc, char *argv[], std::string& archiveName,
                    std::vector<std::string>& inputList, std::string& flag, int& version){
	if (argc < 3) return false;
	if ((std::string)argv[1] == "-c" && argc < 4) return false;   // checking if -c flag used correctly
	std::set<std::string> vFlag = {"-c", "-a", "-x", "-m", "-p"}; // set of valid flags
	if (vFlag.find(argv[1]) != vFlag.end()== false) {             // checks if the entered flag is valid
		return false;
	}
	// checking if -x flag used correctly
	if ((std::string)argv[1] == "-x") {
		if ((std::string)argv[2] == "-o" && argc >= 6) {
			flag = (std::string)argv[1];
			version = std::stoi(argv[3]);
			archiveName = (std::string)argv[4];
			for (int i = 5; i < argc; i++) {
				inputList.push_back(argv[i]);
			}
			return true;
		}
	}
	/* load the inputList with the sequence of files/folders to archive or unarchive */
	for (int i = 3; i < argc; i++) {
		inputList.push_back(argv[i]);
	}
	flag = (std::string)argv[1];
	archiveName = (std::string)argv[2];
	version = -1; // When no version extract is explicitely defined
	if(archiveName.substr(archiveName.find_last_of(".") + 1) != "ad") {
		std::cerr << "Archive file must end with a .ad extension" << std::endl;
		return false;
	}
	return true;
}
