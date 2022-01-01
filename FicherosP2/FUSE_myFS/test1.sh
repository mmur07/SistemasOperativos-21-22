#!/bin/bash

MPOINT="./mount-point"

rm -R -f test
mkdir test
rm -R -f mount-point
mkdir mount-point

echo "Copying file 1"
cp ./src/fuseLib.c $MPOINT/
cp ./src/fuseLib.c ./test
read -p "Press enter..."

echo "Creating file 2"
cp ./src/myFS.h $MPOINT/
cp ./src/myFS.h ./test
read -p "Press enter..."

echo "Disk auditioning"
./my-fsck virtual-disk
if diff ./test/myFS.h $MPOINT/myFS.h
then echo echo $"CORRECTO: Son iguales :)"
else echo $"FALLO: No son iguales :("
fi

