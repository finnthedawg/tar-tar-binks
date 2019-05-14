#ifndef APPEND_H // Making sure the header files are not included twice
#define APPEND_H

/* function to append to the archive file */
int append_archive (std::vector<std::string> inputList, std::fstream &archivePtr,
                    struct Header &mainHeader, std::vector<struct Metadata>& metaVector, char flag);

#endif
