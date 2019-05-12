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

	/* file information stored in inode */
	char fileName[FILENAME_MAX];
	char userID[20];
	char groupID[20];
	int filePermission;
	long long fileSize;
	int numberOfLinks;
	char accessDate[40];
	char modifyDate[40];
	char changeDate[40];
	char birthDate[40];
	int inode;
};

/* Defining functions here */

/* Recursive iteration through directories in disk and add the information about the file/directory to
    the metadata vector and Immediately write to archive file on disk if its a file */
void iterate_through_dir(std::string baseDirName, std::fstream &archivePtr, struct Header &mainHeader);

/* Write header to disk */
int write_header_to_disk(struct Header &mainHeader, std::fstream &archivePtr);

/* Add updated version or new Meta struct to Metadata in memory when using -a APPEND flag */
/* When using -a append flag, search through Global Metadata Struct to see if a prev ver exists
   Push an updated version meta to vector if found
   Push a new meta to vector if not found */
int update_metadata_in_memory(std::vector <struct Metadata> &metaVector, std::fstream &readFilePtr);

/* Append/Write the file to the archive on disk for -c CREATE and -a APPEND flag */
int append_file_to_disk(std::fstream &archivePtr, std::string pathToObject, struct Header &mainHeader);

#endif
