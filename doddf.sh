td="/home/dhugh/testdd"
rm -rf $td
mkdir $td
mkdir $td/sub1
mkdir $td/sub1/sub2
cd $td
dd if=/dev/urandom of=$td/file1 bs=1024 count=1
dd if=/dev/urandom of=$td/sub1/file2 bs=1024 count=1
dd if=/dev/urandom of=$td/sub1/sub2/file3 bs=10240 count=1
cp -p $td/file1 $td/sub1/dfile1file4
cp -p $td/sub1/file2 $td/dfile2file5
cp -p $td/sub1/sub2/file3 $td/sub1/dfile3file6
touch $td/emptyfile11
