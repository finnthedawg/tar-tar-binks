/*
 * Function and variable naming convention used
 * all variables = camelCase
 * all function names = function_do_undscr()
 * structs
 */
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include "common.h"

/* function to check for valid inputs and store archiveFile name in archiveFile
    and inputDir name in inputDir, tartar flag as flag and the version number */
bool chk_cmd_args(int argc, char *argv[], std::string& archiveFile,
                  std::string& inputDir, std::string& flag, int& version);

int main(int argc, char *argv[]) {
	std::string archiveFile, inputDir, flag;
	int version;
	/* Checking for valid cmd args */
	if (chk_cmd_args(argc, argv, archiveFile, inputDir, flag, version) == false ) {
		// incorrect args error exit
		std::cerr << "Incorrect Usage: tartar -flag <archive-file> <file/directory list>" << std::endl;
		return 0;
	}

	std::cout << "DEBUG TARTAR WILL NOW COMMENCE\n" \
	          << archiveFile <<" " << inputDir<< " "<< version << " Should have smth before" << std::endl;




	return 0;
}

/* Function to check for illegal args while invoking the tar archiver */
bool chk_cmd_args(int argc, char *argv[], std::string& archiveFile,
                  std::string& inputDir, std::string& flag, int& version){
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
		archiveFile = (std::string)argv[4];
		inputDir = (std::string)argv[5];
		version = std::stoi(argv[3]);
		return true;
	}
	if ( argc != 4 ) {
		return false;
	}
	flag = (std::string)argv[1];
	archiveFile = (std::string)argv[2];
	inputDir = (std::string)argv[3];
	version = -1; // When no version extract is explicitely defined

	return true;
}
