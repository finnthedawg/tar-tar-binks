
#./adtar {-c|-a|-x|-m|-p| -o NUMBER} <archive-file> <file/directory list>

echo "#####################################################"
echo "Setting up..............."
echo "#####################################################"
rm -r mydir.org 2> /dev/null
rm -r mydir 2> /dev/null


echo "cp /tmp/mydir.tar ~"
cp /tmp/mydir.tar  .

echo "tar xvf mydir.tar"
tar xvf mydir.tar

echo "Renaming mydir to mydir.org for future comparison"
echo "mv mydir mydir.org"
mv mydir mydir.org

echo "------------------------------------------------------------"
echo
echo
echo "extract again mydir."
echo "WARNING: Please dont use cp command to create backup dir mydir.org"
echo
echo
echo "------------------------------------------------------------"

echo "tar xvf mydir.tar"
tar xvf mydir.tar
echo "################################################################"
echo "Create(-x) .ad file and test it after the extraction(-x)"
echo "################################################################"


echo "adtar -c mydir.ad  mydir"
./adtar -c mydir.ad  mydir

echo "Delete the original dir"
echo "rm -r mydir"
rm -r mydir 

echo "------------------------------------------------------------"
echo "Extract mydir from mydir.ad"
echo "adtar -x mydir.ad"
./adtar -x mydir.ad  

echo "------------------------------------------------------------"
echo "After the extraction Compare the file permission, gid/owner id, file size, file name"
echo

echo "ls -l mydir/levelA_1/levelB_1/levelC_1/myfile2"
ls -l mydir/levelA_1/levelB_1/levelC_1/myfile2
#-rw-------. 1 nr83 nr83 335544310 May  9 11:35 mydir/levelA_1/levelB_1/levelC_1/myfile2

echo
echo "Compare it with mydir.org"
echo
echo "ls -l mydir.org/levelA_1/levelB_1/levelC_1/myfile2"
#ls -l mydir.org/levelA_1/levelB_1/levelC_1/myfile2
#-rw-------. 1 nr83 nr83 335544310 May  9 10:35 mydir.org/levelA_1/levelB_1/levelC_1/myfile2


echo "------------------------------------------------------------"
echo "check number of hardlinks."
echo "number of hardlinks should be 3 after the extraction"
echo

echo "ls -l mydir/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1"
ls -l mydir/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1
#-rwxr--r--. 3 nr83 nr83 1677721550 May  9 11:03 mydir/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1

echo
echo "Compare it with mydir.org"
echo

echo "ls -l mydir.org/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1"
ls -l mydir.org/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1
#-rwxr--r--. 3 nr83 nr83 1677721550 May  9 10:03 mydir.org/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1


echo "------------------------------------------------------------"
echo "Check softlinks"
echo
echo "ls -l mydir/levelA_2/levelB_1/levelC_1/hmyfile1"
ls -l mydir/levelA_2/levelB_1/levelC_1/hmyfile1
#lrwxrwxrwx. 1 nr83 nr83 44 May  9 14:13 mydir/levelA_2/levelB_1/levelC_1/hmyfile1 -> ../../../levelA_1/levelB_1/levelC_1/hmyfile1

echo
echo "Compare it with mydir.org"
echo

echo "ls -l mydir.org/levelA_2/levelB_1/levelC_1/hmyfile1"
ls -l mydir.org/levelA_2/levelB_1/levelC_1/hmyfile1
#lrwxrwxrwx. 1 nr83 nr83 44 May  9 13:13 mydir.org/levelA_2/levelB_1/levelC_1/hmyfile1 -> ../../../levelA_1/levelB_1/levelC_1/hmyfile1
echo "------------------------------------------------------------"


echo "Check with checksum"

echo "cksum mydir/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1"
cksum mydir/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1
#2241730624 1677721550 mydir/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1

echo
echo "Compare it with mydir.org"
echo

echo "cksum mydir.org/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1"
cksum mydir.org/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1
#2241730624 1677721550 mydir.org/levelA_3/levelB_1/levelC_1/levelD_1/levelE_1/hmyfile1

echo "------------------------------------------------------------"

echo "################################################################"
echo "Append the existing directory after modifying specific file "
echo "################################################################"


echo "echo \"new content appended\" >> mydir/levelA_1/levelB_1/levelC_1/hello.txt"
echo "echo \"new file added\" > mydir/levelA_1/levelB_1/hello.txt"
echo "new content appended" >> mydir/levelA_1/levelB_1/levelC_1/hello.txt
echo "new file added" > mydir/levelA_1/levelB_1/hello.txt

echo "----------------------------------------------------------------"
echo "Append again mydir with a new file and one modified file"
echo "adtar -a mydir.ad  mydir"
./adtar -a mydir.ad  mydir

echo "----------------------------------------------------------------"
echo "Remove mydir before extracting the folder"
echo "rm -r mydir"
rm -r mydir

echo "----------------------------------------------------------------"
echo "Extract initial version of hello.txt"
echo "./adtar -x -o 1 mydir.ad   mydir/levelA_1/levelB_1/levelC_1/hello.txt"
./adtar -x -o 1 mydir.ad   mydir/levelA_1/levelB_1/levelC_1/hello.txt
echo "cat mydir/levelA_1/levelB_1/levelC_1/hello.txt"
cat mydir/levelA_1/levelB_1/levelC_1/hello.txt

rm -r mydir
echo "----------------------------------------------------------------"
echo "Extract the latest version of hello.txt"
echo "./adtar -x  mydir.ad   mydir/levelA_1/levelB_1/levelC_1/hello.txt"
./adtar -x mydir.ad mydir/levelA_1/levelB_1/levelC_1/hello.txt
echo "cat mydir/levelA_1/levelB_1/levelC_1/hello.txt"
cat mydir/levelA_1/levelB_1/levelC_1/hello.txt

rm -r mydir
echo "----------------------------------------------------------------"
echo "Extract new file  hello.txt added"
echo "./adtar -x  mydir.ad   mydir/levelA_1/levelB_1/hello.txt"
./adtar -x  mydir.ad   mydir/levelA_1/levelB_1/hello.txt
echo "cat mydir/levelA_1/levelB_1/hello.txt"
cat mydir/levelA_1/levelB_1/hello.txt




echo "################################################################"
echo "print meta data: choose your own format "
echo "################################################################"
./adtar -m  mydir.ad

echo "################################################################"
echo "print hierarchy: choose your own format "
echo "################################################################"
./adtar -p  mydir.ad
