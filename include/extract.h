#ifndef EXTRACT_H // Making sure the header files are not included twice
#define EXTRACT_H


#endif

/* Extracts version, or if -1, extracts the newest version only */
int extract_archive_version(std::fstream &archivePtr, std::vector<struct Metadata> &metaVector, int version);

/* Extracts one file or directory */
int extract_meta_file(std::fstream &archivePtr, struct Metadata &meta);
