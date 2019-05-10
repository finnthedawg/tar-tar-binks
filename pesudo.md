# tar-tar archiver pseudo code

## Header structure

```C++
struct Header {
    long long offsetToMeta;       // Byte offset to end of files or start of metadata
    int fileCount;                // Number of files/folders
}
```

## Metadata structure

```C++
struct Metadata {
    int file;                    // 1 if metadata refers to a file otherwise 0
    int directory;               // 1 if metadata refers to a folder otherwise 0
    char pathToObject[FILENAME_MAX];
    int version;                 // version of file/directory
    long long offsetToNext;      // TODO offset to next Metadata struct object
    struct Metadata next = NULL; // TODO pointer to the next metadata struct object

    /* file information stored in inode */
    int filePermission;
    int numberOfLinks;
    char owner[1024];
    char groupOwner[1024];
    long long fileSize;
    char date[30];
    char fileName[FILENAME_MAX]
}
```

### tar-tar operations

#### Recursive iteration through directories in disk

```C++
// using dirent.h
void iterate_through_dir(string dirName) {
    for each file in dirName:
        //check if dirName is actually a file
        if ( dirName == File ) {
            // TODO Open file pointer
            read_file_pointer = open(dirName)

            /* TODO The append_to_metadata func does the following
             - Immediately write file to disk designate d by the header in header pointer
             - extract Metadata and write to Metadata vector */
            append_to_metadata(vector <struct Metadata>* metaVector, read_file_pointer)
        }
        // TODO if dirName is a folder
        if ( dirName == Directory ) {
            update_metadata_in_memory(vector <struct Metadata> *metaVector, read_file_pointer)

            // recursively call the recursion function
            iterate_through_dir(dirName)
        }
}
```

#### Initialize tar-tar-archive in memory

```C++
initialize_file(string tarPath){
    open(tarPath)
    struct Header head;
    struct Metadata meta;
    write(head)
    write(meta)
}
```

#### Read Metadata from disk

```C++
vector <struct Metadata> read_metadata_from_disk(long long offset){
    fseek(offset) // Seek to the offset of metadata Header.offsetToMeta
    vector <struct Metadata> metaVector;
    do {
        meta = read(size struct Metadata);
        metadataVector.push(meta);
    } while (meta != NULL);
    return metaVector
}
```

#### Write metadata to disk

```C++
/* Call this function at the very end */
write_metadata_to_disk(long long offset, vector <struct Metadata>){
    fseek(offset)
    for each meta {
    // write to offset
    }
}
```

#### Update Version in Metadata or add to it in memory

```C++
/* Search through Global Metadata Struct to update version.
   Push to vector if not found */
update_metadata_in_memory(vector <struct Metadata> *metaVector, read_file_pointer) {
    // TODO Search through the Global Metadata Struct to update the version
    // TODO if not found, push to vector
}
```

#### Append to Metadata in memory and Add New Files to archive file on disk

```C++
append_to_metadata(vector <struct Metadata>* metaVector, read_file_pointer){
    /* Search through global metadata Struct to update version.
       Push to vector if not found */
    update_metadata_in_memory(vector <struct Metadata> *metaVector, read_file_pointer)

    //Calls
    append_file_to_disk(file pointer)
}
```

#### Utility functions

##### Append/Write the file to the archive on disk

```C++
append_file_to_disk(file_file_pointer){
    // TODO appends file to current pffsetToMeta
    // TODO Update the offsetToMeta offset
}
```

### Check for hardlinks and softlinks

-   discard hard/soft links outside the scope of the directory that is being archived
