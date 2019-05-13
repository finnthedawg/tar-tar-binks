#ifndef STORE_H // Making sure the header files are not included twice
#define STORE_H

/* function to create an archive file */
int store_archive(std::vector<std::string> inputList, std::fstream &archivePtr,
                  struct Header &mainHeader, std::vector<struct Metadata> &metaVector, char flag);

#endif
