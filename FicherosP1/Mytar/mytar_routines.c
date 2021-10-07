#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

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
	char * buffer = malloc(sizeof(char)* nBytes);
	while(copiedBytes < nBytes && fread(buffer, nBytes * sizeof(char), 1, origin) != 0){
		if (fwrite(buffer, nBytes * sizeof(char), 1, destination) == 0){
			if (ferror(destination)!= 0) return -1;
		}
		copiedBytes += nBytes * sizeof(char);
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
		} while (!feof(file));

		fseek(file, -bytesRead, SEEK_CUR);

		char* string = malloc(sizeof(char) * bytesRead);

		fread(string, sizeof(char), bytesRead, file);
		
		int i = 0;
		do {
			fread(&c, sizeof(char), 1, file);
			string[i] = c;
			i++;
		} while (c != 0 && c != '\0');

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
	numero de archivos
	---------------------
	nombre de archivo
	---------------------
	tamaño de archivo
	----------------------
	.........
	----------------------
	contenido del archivo
 */

int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	int *fileSizes = malloc(sizeof(int) * nFiles);
	FILE* tarFile = fopen(tarName, "wb");
	int offset = sizeof(int) * (nFiles + 1); //Número de archivos + Tamaño de los archivos

	for(int currentFile = 0; currentFile < nFiles; currentFile++){
		offset+= sizeof(char) * strlen(fileNames[currentFile]) + 1;
	}

	fseek(tarFile, offset, SEEK_CUR);

	for(int currentFile = 0; currentFile < nFiles; currentFile++){
		FILE* readFile = fopen(fileNames[currentFile], "r");
		fseek(readFile, 0, SEEK_END);
		fileSizes[currentFile] = ftell(readFile);
		rewind(readFile);
		copynFile(readFile, tarFile, fileSizes[currentFile]);
		fclose(readFile);
	}

	fseek(tarFile, 0, SEEK_SET);	//ITS REWIND TIME
	fwrite(&nFiles, sizeof(int), 1, tarFile); //Numero archivos

	for(int currentFile = 0; currentFile < nFiles; currentFile++){
		fwrite(fileNames[currentFile], sizeof(char) * strlen(fileNames[currentFile]) + 1, 1, tarFile);
		fwrite(&fileSizes[currentFile], sizeof(int), 1, tarFile);
	}

	fclose(tarFile);

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
	FILE *tarFile = fopen(tarName, "r");

	unsigned char* c;
	int nFiles;

	stHeaderEntry *p;
	p = (stHeaderEntry *) malloc(sizeof(stHeaderEntry) * (nFiles));

	//Leemos los datos de cabecera y lo almacenamos en p
	p = readHeader(tarFile, &nFiles);

	int i;
	for (i = 0; i < nFiles; i++) {
		//Creamos nuevo fichero con mismo nombre en modo escritura, leemos el contenido y lo copiamos en el nuevo fichero
		FILE *nuevo = fopen(p[i].name, "w");
		if (nuevo != NULL) {
			c = (unsigned char*) malloc(p[i].size);
			fread(c, sizeof(unsigned char) * p[i].size, 1, tarFile); //leemos de tarFile
			fwrite(c, sizeof(unsigned char) * p[i].size, 1, nuevo);	//escribimos en el nuevo archivo
			fclose(nuevo);
		} else
			return EXIT_FAILURE;
	}
	fclose(tarFile);

	printf("\nFile successfully extracted!\n\n");

	return EXIT_SUCCESS;
}
