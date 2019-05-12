/*
 * Function and variable naming convention used
 * all variables = camelCase
 * all function names = function_do_undscr()
 * structs
 */
#include <set>
#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include <csignal>
#include <iostream>
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
bool chk_cmd_args(int argc, char *argv[], std::string& archiveName,
                  std::vector<std::string>& inputList, std::string& flag, int& version);

/* Signal Handler for catching SIGABRT signals */
void signalHandler( int signum );

int main(int argc, char *argv[]) {
	// register signal SIGSEGV and signal handler
	signal(SIGABRT, signalHandler);
	std::string archiveName, flag;      // name of archive file, flag used
	std::vector<std::string> inputList; // vector of folder/files to archive/check version or extract
	int version;                        // will be equal to -1 if -o version number is not specified
	Header mainHeader;
	std::vector<struct Metadata> metaVector;

	/* Checking for valid cmd args */
	if (chk_cmd_args(argc, argv, archiveName, inputList, flag, version) == false ) {
		// incorrect args error exit
		std::cerr << "Incorrect Usage: ./tartar -flag <archive-file> <file/directory list>" << std::endl;
		return 1;
	}

	std::fstream archivePtr;
	archivePtr.open(archiveName, std::fstream::in |
	                std::fstream::out | std::fstream::app); // open archiveName with rd, wrt and append perm
	if ( !(archivePtr.is_open()) ) {
		std::cerr << "ERROR: cannot open "<< archiveName << std::endl;
		return 1;
	}

	std::cout << "DEBUG TARTAR WILL NOW COMMENCE\n" \
	          << "Archive Name is " <<archiveName << ". Version is "<< version << std::endl;

	// Check the flag type and run the appropriate command
	switch (flag[1]) {
	case 'c': // -c stpre flag
		std::cout << "DEBUG -c flag used. Files/dirs are:" << '\n';
		// iterating through the inputList vector to get all the files/dirs to archive
		for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
			std::cout << "DEBUG " << inputList[i] << std::endl;
			iterate_through_dir(inputList[i], archivePtr, mainHeader);
		}
		break;
	case 'a': // -a append flag
		std::cout << "DEBUG -a flag used. Files/dirs are:" << '\n';
		// iterating through the inputList vector to append all the stated files
		for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
			std::cout << "DEBUG " << inputList[i] << std::endl;

			// TODO run the append function for each file/dir in the inputList
		}
		break;
	case 'x': // -x extract flag with/without -o version flag
		std::cout << "DEBUG -x flag used. Files/dirs are:" << '\n';
		// iterating through the inputList vector to extract all the stated files/ non-empty dirs
		for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
			std::cout << "DEBUG " << inputList[i] << std::endl;

			// TODO run the extract function for each file/dir in the inputList
		}
		break;
	case 'm': // -m print metatdata flag
		// TODO print out meta data
		std::cout << "DEBUG -m flag." << '\n';

		/* If no files are specified then print out Metadata of all files in archive */
		if (argc == 3) {
			/* code */
		}
		// iterating through the inputList vector to print the metadata for all mentioned files/folders
		for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
			std::cout << "DEBUG " << inputList[i] << std::endl;

			// TODO run the extract function for each file/dir in the inputList
		}
		break;
	case 'p': // -p print directory hierarchy flag
		// TODO print the directory hierarchy
		std::cout << "DEBUG -p flag." << '\n';
		break;
	default:
		std::cout << "ERROR: invalid flags" << '\n';
	}
	archivePtr.close();
	return 0;
}

void signalHandler( int signum ) {
	std::cerr << "Interrupt signal (" << signum << ") received.\n";
	std::cerr << "Correct Usage: ./tartar -flag <archive-file> <file/directory list>" << std::endl;
	// cleanup and close up stuff here
	// terminate program
	exit(signum);
}

/* Function to check for illegal args while invoking the tar archiver */
bool chk_cmd_args(int argc, char *argv[], std::string& archiveName,
                  std::vector<std::string>& inputList, std::string& flag, int& version){
	if (argc < 3) {
		return false;
	}

	std::set<std::string> vFlag = {"-c", "-a", "-x", "-m", "-p"}; // set of valid flags
	const bool isValidFlag = vFlag.find(argv[1]) != vFlag.end();  // checks if the entered flag is valid

	if (isValidFlag == false) {                                   // checking for invalid flags
		return false;
	}
	// checking if -x flag used correctly
	if ((std::string)argv[1] == "-x" ) {
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

	for (int i = 3; i < argc; i++) {
		inputList.push_back(argv[i]);
	}
	flag = (std::string)argv[1];
	archiveName = (std::string)argv[2];
	version = -1; // When no version extract is explicitely defined

	return true;
}
