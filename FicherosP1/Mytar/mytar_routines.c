#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

const int BUFF_SIZE = 1;

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	int copiedBytes = 0;
	char * buffer = malloc(sizeof(char)* BUFF_SIZE);
	while(copiedBytes < nBytes && fread(buffer, BUFF_SIZE * sizeof(char), 1, origin) != 0){
		if (fwrite(buffer, BUFF_SIZE * sizeof(char), 1, destination) == 0){
			if (ferror(destination)!= 0) return -1;
		}
		copiedBytes += BUFF_SIZE * sizeof(char);
	}
	if (ferror(origin)!= 0) return -1;
	return copiedBytes;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file) //No se si funciona aun pero creo que falta ponerle un '\0' al final del string.
{
	int bytesRead = 0;
	char c;
	if (file != NULL){ //Búsqueda del fin del archivo. Se calcula cuanto mide.
		do{
			fread(&c, sizeof(char), 1, file);
			bytesRead++;
		} while (!feof(file) && c != '\0');

		rewind(file);

		char* string = malloc(sizeof(char) * bytesRead);

		fread(string, sizeof(char), bytesRead, file);

		return string;
	}

	return NULL;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	stHeaderEntry *header;
	int _nFiles;
	fread(&_nFiles, sizeof(int), 1, tarFile);
	
	*nFiles = _nFiles;

	header = malloc(sizeof(stHeaderEntry) * _nFiles);
	for (int i = 0; i < _nFiles; i++){
		header[i].name = loadstr(tarFile);
		fread(&header[i].size, sizeof(int), 1, tarFile);
	}
	
	return header;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE* files[nFiles];
	int fileSizes[nFiles];

	FILE* tarFile = fopen(tarName, "wb");
	stHeaderEntry* header = malloc(sizeof(stHeaderEntry) * nFiles);
	int offset = 0;
	for(int currentFile = 0; currentFile < nFiles; currentFile++){
		//strcpy(header[currentFile].name, &tarName[currentFile]); //Anota el path de cada archivo
		header[currentFile].name = tarName[currentFile];
		offset += strlen(fileNames[currentFile] + 1);
	}
	
	fseek(tarFile, sizeof(int) * nFiles + sizeof(char) * offset, SEEK_SET);

	for(int currentFile = 0; currentFile < nFiles; currentFile++){
		FILE* readFile = fopen(fileNames[currentFile], "r");
		fseek(readFile, 0, SEEK_END);
		header[currentFile].size = ftell(readFile);
		//fileSizes[currentFile] = copynFile(files[currentFile], tarFile, )
	}

	// for (int currentFile = 0; currentFile < nFiles; currentFile++){ //Lectura de tamaño y nombres de los archivos.
	// 	files[currentFile] = fopen(fileNames[currentFile], "r");
	// 	fileSizes[currentFile] = loadstr(fileNames[currentFile]);
	// }

	// 
	// fwrite(&nFiles, sizeof(int), 1, tarFile); //Escribe el número total de ficheros.
	// for(int currentFile = 0; currentFile < nFiles; currentFile++){ //Escribe por cada archivo su ruta(nombre) y su tamanyo
	// 	fwrite(fileNames[currentFile], sizeof(char), strlen(fileNames[currentFile] + 1), tarFile); 
	// 	fwrite(fileSizes[currentFile], sizeof(int), 1, tarFile);
	// }
	// for(int currentFile = 0; currentFile < nFiles; currentFile++){ //Escribe los datos de cada archivo
	// 	copynFile(files[currentFile], tarFile, fileSizes[currentFile]);
	// 	fclose(files[currentFile]);
	// }
	// fclose(tarFile);
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	// Complete the function
	return EXIT_FAILURE;
}
