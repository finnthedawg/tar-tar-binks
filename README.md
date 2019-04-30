# tar-tar-binks
A Unix-based tar facility that handles file collisions within the tarball.

## Build (OSX/Linux)
```shell
$ make all
```

### Acknowledgements
-   Yet to add
=======
## Run instructions
```
>> tartar {-c | -a | -x | -m | -p | -o NUMBER} <archive-file> <file/directory list>
```

`-c` store in the archive file <archive-file>, the files and/or directories provided by the list <file/directory list>.
If other additional files/directories exist in <directory list>, then all this content is recursively stored in the <archive-file>

`-a` append filesystem entities indicated in the <file/directory list> in the archived file <archive-file> that already
exists. If additional files and directories exist in <file/directory list>, they are recursively appended in the
designated with <archive-file>.

`-x` extract all files and catalogs that have been archived in file <archive-file>. **If -o specified, then extract the files
with NUMBERth occurrence of each file.** If option -o is not specified it will extract the latest occurrence

`-m` print out the meta-data (owner, group, rights, type of file) for all files/directories that have been archived in <archive-file>.

`-p` display the hierarchy(-ies) of the files and directories stored in the <archive-file>.
>>>>>>> 70affddc0cfedfe322a11cb8d3e7c7ae4eb69998
