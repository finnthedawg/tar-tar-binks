# tar-tar archiver pseudo code

## Header structure
```C++
struct Header{
    long long offsetToMeta; //Byte offset to end of files
    int fileCount; //Number of files
}
```

## Metadata structure
```C++
struct Metadata {
    int file;        // 1 if metadata refers to a file otherwise 0
    int directory;   // 1 if metadata refers to a folder otherwise 0
    char pathToObject[FILENAME_MAX];
    int version;     // version of file/directory

    /* file information stored in inode */
    int filePermission;
    int numberOfLinks;
    char owner[1024];
    char groupOwner[1024];
    long long int fileSize;
    char date[30];
    char fileName[FILENAME_MAX]
}
```
### tar-tar operations

#### Initialize file
```C++
initialize_file(string tarPath){
    open(tarPath)
    struct Header head;
    struct Metadata meta;
    write(head)
    write(meta)
}
```

#### Read Metadata
```C++
vector <struct Metadata> read_metadata_from_disk(long long offset){
    fseek(offset) //Seek to the offset of metadata
    vector <struct Metadata> metaVector;
    do{
        meta = read(size struct Metadata);
        metadataVector.push(meta);
    } meta != NULL
    return(metaVecvtor)
}
```

#### Write metadata
```C++
write_metadata_to_disk(long long offset, vector <struct Metadata>){
    fseek(offset )
    for each meta{
    //write to offset
    }
}
```

#### Appending new files
```C++
append_to_metadata(vector <struct Metadata>* metaVector, read_file_pointer){
    //Search through global metadata Struct to update version.
    //Push to vector if not found
    //Calls
    append_file_to_disk(file pointer)
}
```

##### utility function
```C++
append_file_to_disk(file pointer){
    //appends file to current head of files
}
```

### Check for hardlinks and softlinks

*   discard hard/soft links outside the scope of the directory that is being archived
