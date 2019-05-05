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

bool chk_cmd_args(int argc, char *argv[], std::string& archiveFile, std::string& inputDir);

int main(int argc, char *argv[]) {
	std::string archiveFile, inputDir;
	/* Checking for valid cmd args */
	if (chk_cmd_args(argc, argv, archiveFile, inputDir) == false ) {
		// incorrect args error exit
		std::cerr << "Incorrect Usage: tartar -flag <archive-file> <file/directory list>" << '\n';
		return 0;
	}

	std::cout << "DEBUG TARTAR WILL NOW COMMENCE\n" \
		<< archiveFile << inputDir << "should have smth before" << std::endl;




	return 0;
}

/* Function to check for illegal args while invoking the tar archiver */
bool chk_cmd_args(int argc, char *argv[], std::string& archiveFile, std::string& inputDir) {
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
		archiveFile = (std::string)argv[4];
		inputDir = (std::string)argv[5];
		return true;
	}
	if ( argc != 4 ) {
		return false;
	}
	archiveFile = (std::string)argv[2];
	inputDir = (std::string)argv[3];
	return true;
}
