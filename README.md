# tar-tar-binks

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/956289d5c77c4785836cac7e895bc60a)](https://www.codacy.com/app/samhunsadamant/tar-tar-binks?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=finnthedawg/tar-tar-binks&amp;utm_campaign=Badge_Grade)

A BSD-based tar utility that flattens out the file hierarchy and creates a archived package that can be inspected and unarchived for later use.

## Build
```shell
$ make all
```

## Clean
```shell
$ make clean
```

## Run instructions (Darwin Systems only for now)
Each flag option can only be run one at a time except for when -x is used with -o.
```shell
$ ./adtar {-c | -a | -x | -m | -p | -o NUMBER} <archive-file> <file/directory list>
```

## Flag options

-   `-c` stores in the archive file <archive-file>, the files and/or directories provided by the list <file/directory list>.
If other additional files/directories exist in <directory list>, then all this content is recursively stored in the \<archive-file>. i.e. `./adtar -c archive_example.ad obj1_to_archive obj2_to_archive`.

-   `-a` appends filesystem entities indicated in the <file/directory list> in the archived file <archive-file> that already
exists. If additional files and directories exist in <file/directory list>, they are recursively appended in the
designated with \<archive-file>. i.e. `./adtar -a archive_example.ad obj1_to_append obj2_to_append`.

-   `-x` extracts all files and catalogs that have been archived in file <archive-file>. **If `-o` specified, then adtar will extract the files
with NUMBER<sup>th</sup> occurrence of each file.** If option -o is not specified it will extract the latest occurrence. i.e. `./adtar -x archive_example.ad`.

-   `-m` prints out the meta-data (owner, group, rights, type of file) for all files/directories that have been archived in \<archive-file>. i.e. `./adtar -m archive_example.ad`.

-   `-p` displays the hierarchy(-ies) of the files and directories stored in the \<archive-file>. i.e. `./adtar -p archive_example.ad`.

### Authors
-   @SamSamhuns
-   @finnthedawg
