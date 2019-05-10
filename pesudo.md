### tar file operations

Initialize file
```
initialize_file(string tarPath){
  Open(tarPath)
  struct Header head;
  struct Metadata meta;
  Write(head)
  Write(meta)
}
```

Read Metadata
```
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

Write metadata
```
write_metadata_to_disk(long long offset, vector <struct Metadata>){
  fseek(offset )
  for each meta{
    //write to offset
  }
}
```

### Appending new files

```
append_to_metadata(vector <struct Metadata>* metaVector, read_file_pointer){
  //Search through global metadata Struct to update version.
  //Push to vector if not found
  //Calls
  append_file_to_disk(file pointer)
}
```

utility function
```
append_file_to_disk(file pointer){
  //appends file to current head of files
}
```

### Header structure

```
struct Header{
  long long offsetToMeta; //Byte offset to end of files
  int fileCount; //Number of files
}
```
