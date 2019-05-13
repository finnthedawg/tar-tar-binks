#ifndef META_H // Making sure the header files are not included twice
#define META_H

/* Implementation for -m flag*/
int display_metadata_from_archive (std::vector <struct Metadata> &metaVector, std::string &pathToObject, bool displayAll);

/* display all attributes on Metadata object */
void display_one_metadata_attrb (struct Metadata &metadata_object);
#endif
