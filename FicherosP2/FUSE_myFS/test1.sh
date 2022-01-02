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
then echo $"CORRECTO: Son iguales :)"
else echo $"FALLO: No son iguales :("
fi
read -p "Press enter..."

echo "Comparing truncated files"
./my-fsck virtual-disk
truncate --size=4096 $MPOINT/myFS.h
truncate --size=4096 test/myFS.h

if diff ./test/myFS.h $MPOINT/myFS.h
then echo $"CORRECTO: Son iguales :)"
else echo $"FALLO: No son iguales :("
fi
read -p "Press enter..."

echo "Copying file 3"
cp ./src/fuseLib.h $MPOINT/
cp ./src/fuseLib.h ./test
read -p "Press enter..."

echo "Comparing file 3"
./my-fsck virtual-disk
if diff ./test/fuseLib.h $MPOINT/fuseLib.h
then echo $"CORRECTO: Son iguales :)"
else echo $"FALLO: No son iguales :("
fi
read -p "Press enter..."

echo "Truncating file 3"
./my-fsck virtual-disk
truncate --size=+4096 $MPOINT/fuseLib.h
truncate --size=+4096 test/fuseLib.h
read -p "Press enter..."

echo "Comparing file 3"
./my-fsck virtual-disk
if diff ./test/fuseLib.h $MPOINT/fuseLib.h
then echo $"CORRECTO: Son iguales :)"
else echo $"FALLO: No son iguales :("
fi
