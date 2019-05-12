#ifndef COMMON_H // Making sure the header files are not included twice
#define COMMON_H

#define DEBUG 0  // Debug MACRO 1 = print all debug msgs, 0 = Do not print debug messages

/* struct for header */
struct Header {
	long long offsetToMeta;       // Byte offset to end of files or start of metadata
	int fileCount;                // Number of files
	int directoryCount;              // Number of folders
};

/* struct for metadata */
struct Metadata {
	int file;                     // 1 if metadata refers to a file otherwise 0
	int directory;                // 1 if metadata refers to a folder otherwise 0
	char pathToObject[FILENAME_MAX];
	int version;                  // version of file/directory
	long long offsetToNext;       // TODO offset to next Metadata struct object
	Metadata *next;  // TODO pointer to the next metadata struct object

	/* file information stored in inode */
	int filePermission;
	int numberOfLinks;
	char userID[1024];
	char groupID[1024];
	long long fileSize;
	char date[30];
	char fileName[FILENAME_MAX];
};

/* Defining functions here */

/* Recursive iteration through directories in disk and add the information about the file/directory to
    the metadata vector and Immediately write to archive file on disk if its a file */
void iterate_through_dir(std::string dirName, std::fstream& archivePtr, struct Header);

#endif
