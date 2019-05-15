#ifndef COMMON_H // Making sure the header files are not included twice
#define COMMON_H

#define DEBUG 1  // Debug MACRO 1 = print all debug msgs, 0 = Do not print debug messages

/* struct for header */
struct Header {
	long offsetToMeta;       // Byte offset to end of files or start of metadata
	int fileCount;           // Number of files
	int directoryCount;      // Number of folders
	int symboliclinkCount;   // Number of symbolic links
	int otherFileCount;      // Number of other file types
};

/* struct for metadata */
struct Metadata {
	int file;                     // 1 if metadata refers to a file otherwise 0
	int directory;                // 1 if metadata refers to a folder otherwise 0
	int softlink;                 // 1 if metadata refers to a softlink otherwise 0
	char pathToObject[FILENAME_MAX];
	int version;                  // version of file/directory
	long offsetToFileStart;       // offset to START of file metadata refers to. Value et to -1 for directories
	/* file information stored in inode */
	char fileName[FILENAME_MAX];
	char symLinkTarget[FILENAME_MAX]; // target path of file symlink points to, empty for normal files/folders

	/* uncomment the part below for storing metatdata obj attrbs as chars */
	// char userID[20];
	// char groupID[20];
	// char filePermission[40];
	// char fileSize[40];
	// int numberOfLinks;
	// char accessDate[40];
	// char modifyDate[40];
	// char changeDate[40];
	// char birthDate[40];
	// char inode[40];

	uid_t userID;
	gid_t groupID;
	mode_t filePermission;
	off_t fileSize;
	nlink_t numberOfLinks;
	time_t accessDate;
	time_t modifyDate;
	time_t changeDate;
	time_t birthDate;
	ino_t inode;
};

/* Defining functions here */

/* function to add objects to archive works for both -c and -a flags */
int add_objects_to_archive(std::vector<std::string> inputList, std::fstream &archivePtr,
                           struct Header &mainHeader, std::vector<struct Metadata>& metaVector, char flag);

/* Recursive iteration through directories in disk and add the information about the file/directory to
    the metadata vector and Immediately write to archive file on disk if its a file */
void iterate_through_dir(std::string &baseDirName,
                         std::fstream &archivePtr,
                         struct Header &mainHeader,
                         std::vector <struct Metadata> &metaVector,
                         char flag);

/* Read the header from archive */

/* Write header to disk */
int write_header_to_disk(struct Header &mainHeader,
                         std::fstream &archivePtr);

/* Read Metadata from disk */
int read_metadata_from_disk(std::fstream &archivePtr, struct Header &mainHeader, std::vector<struct Metadata> &metaVector);

/* Write Metadata to disk */
/* Call this function at the very end */
int write_metadata_to_disk(struct Header &mainHeader,
                           std::fstream &archivePtr,
                           std::vector <struct Metadata> &metaVector);

/* Create a new Metadata object */
struct Metadata create_metadata_object(struct Header &mainHeader,
                                       std::string &fileName,
                                       std::string &pathToObject);

/* Add updated version or new Meta struct to Metadata in memory*/
/* When using -a append flag, search through Global Metadata Struct to see if a prev ver exists
   Push an updated version meta to vector if found
   Push a new meta to vector if not found
   For flag = c, push to metaVector directly without checking for old versions */
int update_metadata_in_memory( struct Header &mainHeader,
                               std::string &fileName,
                               std::string &pathToObject,
                               std::vector <struct Metadata> &metaVector,
                               std::fstream &archivePtr,
                               char flag);

/* Append to Metadata in memory and Add New Files to archive file on disk
   This function should only be called for files not directories */
int append_to_metadata(std::string &fileName,
                       std::string &pathToObject,
                       std::vector <struct Metadata> &metaVector,
                       struct Header &mainHeader,
                       std::fstream &archivePtr,
                       char flag);

/* Utility functions */
/* Append/Write the file to the archive on disk for -c CREATE and -a APPEND flag */
int append_file_to_disk(std::fstream &archivePtr,
                        std::string &pathToObject,
                        struct Header &mainHeader);

/* gets file size of a fstream object */
int file_size(std::fstream &fstream_obj);

/* convert UNIX time format to date format */
char * unix_time_to_date (time_t unixTime);

/* converting mode_t to a permission string in format -rwxrwxrwx*/
char * mode_to_permission (mode_t fileMode);

/* converts uid to user name */
std::string get_user_name (uid_t uid);

/* convertes gid to group id */
std::string get_group_name (uid_t gid);

/*  Extracts file name from pathToObject
    Utility code from
   C++ Cookbook by Jeff Cogswell, Jonathan Turkanis, Christopher Diggins, D. Ryan Stephens */
std::string get_filename_from_path(std::string &pathToObject);

#endif
