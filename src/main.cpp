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
#include <iostream>
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

int main(int argc, char *argv[]) {
	std::string archiveName, flag;
	std::vector<std::string> inputList;
	int version; // will be equal to -1 if -o version number is not specified

	/* Checking for valid cmd args */
	if (chk_cmd_args(argc, argv, archiveName, inputList, flag, version) == false ) {
		// incorrect args error exit
		std::cerr << "Incorrect Usage: tartar -flag <archive-file> <file/directory list>" << std::endl;
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
	          << archiveName <<" " << " "<< version << " Should have smth before" << std::endl;
	// iterating through the inputList cector to print everything out
	for(std::vector<int>::size_type i = 0; i != inputList.size(); i++) {
		std::cout << "DEBUG" << inputList[i] << std::endl;
	}



	archivePtr.close();
	return 0;
}

/* Function to check for illegal args while invoking the tar archiver */
bool chk_cmd_args(int argc, char *argv[], std::string& archiveName,
                  std::vector<std::string>& inputList, std::string& flag, int& version){
	if (argc < 4) {
		return false;
	}

	std::set<std::string> vFlag = {"-c", "-a", "-x", "-m", "-p"}; // set of valid flags
	const bool isValidFlag = vFlag.find(argv[1]) != vFlag.end();  // checks if the entered flag is valid

	if (isValidFlag == false) {                                   // checking for invalid flags
		return false;
	}
	// checking if -x flag used correctly
	if ((std::string)argv[1] == "-x" && (std::string)argv[2] == "-o" && argc == 6) {
		flag = (std::string)argv[1];
		version = std::stoi(argv[3]);
		archiveName = (std::string)argv[4];
		for (int i = 5; i < argc; i++) {
			inputList.push_back(argv[i]);
		}
		return true;
	}

	for (int i = 3; i < argc; i++) {
		inputList.push_back(argv[i]);
	}
	flag = (std::string)argv[1];
	archiveName = (std::string)argv[2];
	version = -1; // When no version extract is explicitely defined

	return true;
}
