#!/bin/bash
# --ENCODNG:UTF-8 --
if [ -f mytar ]
then
    if [ -d tmp ]
    then
        rm -r tmp
    fi
    mkdir tmp
    cd tmp
    echo $"Hello world!" > file1.txt
    head "/etc/passwd" > file2.txt
    head -c 1024 "/dev/urandom" > file3.dat

    ../mytar -cf mytar.mtar file1.txt file2.txt file3.dat

    mkdir out
    cp mytar.mtar out

    cd out
    ../../mytar -xf mytar.mtar
    if diff file1.txt ../file1.txt && diff file2.txt ../file2.txt && diff file3.dat ../file3.dat
    then
    cd ../../
    echo $"Correct"
    exit 0
    else
    cd ../../
    echo $"La practica esta mal porque el codigo esta mal (o los archivos son distintos)"
    exit 1
    fi
else
echo $"No se ha encontrado el ejecutable"
exit 1;
fi