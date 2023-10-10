#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>  
#include <string.h>

/*for the ls command*/
void listDir()
{
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		//printf("Current working dir: %s\n", cwd);
		DIR *pDir;
		pDir = opendir(cwd);
		struct dirent *dp;
		while ((dp = readdir(pDir)) != NULL){
            write(1,dp->d_name,strlen(dp->d_name));
            write(1,"\n",1);
    	}
	closedir(pDir);
	} 
	return;
} 

/*for the pwd command*/
void showCurrentDir() 
{
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
        write(1,cwd,strlen(cwd));
        write(1,"\n",1);
	} 
	return;
} 

/*for the mkdir command*/
// create dir in current directory
void makeDir(char *dirName)
{
	printf("%s%s\n","Create ",dirName);
	return;
} 

/*for the cd command*/
// find dir (?), then chane current to this
void changeDir(char *dirName)
{
	printf("%s%s\n","Relocate ",dirName);
	return;
}  

/*for the cp command*/
// find file (open/close?) (or err/fail), find dest (?), cp file to dest
void copyFile(char *sourcePath, char *destinationPath)
{
	printf("%s%s%s%s\n","Copy ",sourcePath," to ",destinationPath);
	return;
}  

/*for the mv command*/
// pos perform copyFile command, then deleteFile command
void moveFile(char *sourcePath, char *destinationPath)
{
	printf("%s%s%s%s\n","Move ",sourcePath," to ",destinationPath);
	return;
}  

/*for the rm command*/
// find file (open/close?) (or err/fail), delete it
void deleteFile(char *filename)
{
	printf("%s%s\n","Remove ",filename);
	return;
}  

/*for the cat command*/
// enter a file (or err/fail), read each line (getline), and print to stdout (write(1,buf,buflen))
void displayFile(char *filename)
{
	printf("%s%s\n","Print ",filename);
	return;
}  