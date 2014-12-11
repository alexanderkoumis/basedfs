#!/bin/bash
MNT=~/mount
sudo chown `whoami`:`whoami` $MNT
cd $MNT
touch newfile
chmod 0777 $MNT/newfile
ls -lah
sudo echo "heythere" > $MNT/newfile

