#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>  
#include <string.h>
#include "string_parser.h"

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
    struct stat st = {0};
    if (stat(dirName, &st) == -1) {
        mkdir(dirName,0700);
    } else {
        write(1,"Error: ",7);
        write(1,dirName,strlen(dirName));
        write(1," already exists!",15);
        write(1,"\n",1);
        return;
    }
	return;
    // should this move the user into the new directory? Or maybe print current?
} 

/*for the cd command*/
// find dir (?), then chane current to this
void changeDir(char *dirName)
{
    struct stat st = {0};
    if (stat(dirName, &st) != -1) {
        chdir(dirName);
    } else {
        write(1,"Error: ",7);
        write(1,dirName,strlen(dirName));
        write(1," does not exist!",16);
        write(1,"\n",1);
        return;
    }
	return;
    // consider printing current directory for feedback's sake
}  

/*for the cp command*/
// find file (open/close?) (or err/fail), find dest (?), cp file to dest
void copyFile(char *sourcePath, char *destinationPath)
{
	//printf("%s%s%s%s\n","Copy ",sourcePath," to ",destinationPath);
    /* continue, as we have a reference point
    - determine whether the passed in source and dest have dir's
    - determine how to separate dir from file.. do we need to?
    - open each to ensure they are valid, otherwise err. Go to source.
    Use Lab 2 code to:
    Open specified file, or err
    Copy to specified dest
    - return to reference point ('prev' dir)
    */
    // consider readdir

    // lookup 'unlink'
    
    //char cwd[PATH_MAX];
	//if (getcwd(cwd, sizeof(cwd)) != NULL) {
    char *buf;
    size_t bufsize = 255;
    size_t charCount;
    buf = (char *)malloc(bufsize * sizeof(char));
    if (buf == NULL)
    {
        perror("Unable to allocate buffer");
        return;
    }
    FILE *freOp;
    FILE *fp;
    //DIR *pDir;
    size_t size = 128;
	char *msg = malloc (size);
    msg = strdup(sourcePath);
    char *filenameSrc;
    char *filenameDest;
    filenameDest = strdup(destinationPath);
    command_line filename_buffer;
    filename_buffer = str_filler (msg, "/");
    //filename = strdup(sourcePath);
    filenameSrc = filename_buffer.command_list[filename_buffer.num_token - 1];
    //pDir = opendir(cwd);
    char *err1;
    char *err2;
    char *err3;
    char *err4;
    char *err5;
    //mode_t mode = S_IWOTH;
    int fe;
    char *trailingDestCharacter = &filenameDest[strlen(filenameDest) - 1];
    //char dir = "/";
    
    //if (getcwd(prev, sizeof(prev)) != NULL) {
        err2 = "Error - failed to open source file: ";
        int fd = open(filenameSrc, O_RDONLY);
        if (fd == -1) {
            write(1,err2,strlen(err2));
            write(1,filenameSrc,strlen(filenameSrc));
            write(1,"\n",1);
        } else {
            //changeDir(destinationPath);
            //changeDir(prev);
            //printf("%s%s\n","Src file: ",filenameSrc);
            if (strcmp("/", trailingDestCharacter)  != 0){
                strcat(filenameDest,"/");
            }
            strcat(filenameDest,filenameSrc);
            //printf("%s%s\n","Dest file: ",filenameDest);
            err3 = "Error - failed to create dest file: ";
            struct dirent *dp;
            freOp = freopen(filenameDest,"w+",stdout);
            fp = fopen(filenameSrc, "r");
            if (fp == NULL) {
					write(1,err2,strlen(err2));
                    write(1,filenameSrc,strlen(filenameSrc));
                    write(1,"\n",1);
            }
            if (freOp != NULL) {
				while((charCount = getline(&buf,&bufsize,fp)) != -1){
						write(1, buf, charCount);
					}
				write(1, "\n", 1);
            }
            int r;
            r = chmod(filenameDest, S_IRWXU );
            err5 = "Error - failed to modify permissions on dest file: ";
            if (r != 0) {
                write(1,err5,strlen(err5));
                write(1,filenameDest,strlen(filenameDest));
                write(1,"\n",1);
            }
            
            fclose(fp);
            //fclose(freOp);
            //close(fd);
            err4 = "Error - failed to close source file: ";
            if (close(fd) < 0) { 
                //perror("c1"); 
                //exit(1); 
                write(1,err4,strlen(err4));
                write(1,filenameSrc,strlen(filenameSrc));
                write(1,"\n",1);
            } 
            // have not yet found how to use getline without fopen (needing a file pointer)
        }
        
    //} else {
    //    err1 = "Error - failed to allocate mem for current directory";
    //    write(1,err1,strlen(err1));
    //}
    free(filenameDest);
    free(msg);
    free(buf);  
    buf = NULL;  
}  

/*for the mv command*/
// pos perform copyFile command, then deleteFile command
void moveFile(char *sourcePath, char *destinationPath)
{
    char *err1;
	copyFile(sourcePath,destinationPath);
    err1 = "Error - failed to find source file: ";
    int fd = open(sourcePath, O_RDONLY);
    if (fd == -1) {
        write(1,err1,strlen(err1));
        write(1,sourcePath,strlen(sourcePath));
        write(1,"\n",1);
    } else {
        unlink(sourcePath);
    }
	return;
}  

/*for the rm command*/
// find file (open/close?) (or err/fail), delete it
void deleteFile(char *filename)
{
    char *filenameSrc;
    filenameSrc = strdup(filename);
    char *err1;
	//printf("%s%s\n","Remove ",filename);
    err1 = "Error - failed to find source file: ";
    int fd = open(filenameSrc, O_RDONLY);
    if (fd == -1) {
        write(1,err1,strlen(err1));
        write(1,filenameSrc,strlen(filenameSrc));
        write(1,"\n",1);
    } else {
        unlink(filenameSrc);
    }
	return;
    // should be able to navigate to directory as well - consider token like copyFile above
}  



/*for the cat command*/
// enter a file (or err/fail), read each line (getline), and print to stdout (write(1,buf,buflen))
void displayFile(char *filename)
{
	char *buf;
    size_t bufsize = 255;
    size_t charCount;
    buf = (char *)malloc(bufsize * sizeof(char));
    if (buf == NULL)
    {
        perror("Unable to allocate buffer");
        return;
    }
    FILE *freOp;
    FILE *fp;
    size_t size = 128;
    char *filenameSrc;
    filenameSrc = strdup(filename);
    char *err2;
    char *err4;
    err2 = "Error - failed to open source file: ";
    int fd = open(filenameSrc, O_RDONLY);
    if (fd == -1) {
        write(1,err2,strlen(err2));
        write(1,filenameSrc,strlen(filenameSrc));
        write(1,"\n",1);
    } else {
        fp = fopen(filenameSrc, "r");
        if (fp == NULL) {
                write(1,err2,strlen(err2));
                write(1,filenameSrc,strlen(filenameSrc));
                write(1,"\n",1);
        }
        if (freOp != NULL) {
            while((charCount = getline(&buf,&bufsize,fp)) != -1){
                    write(1, buf, charCount);
                }
            write(1, "\n", 1);
        }
        fclose(fp);
        err4 = "Error - failed to close source file: ";
        if (close(fd) < 0) { 
            write(1,err4,strlen(err4));
            write(1,filenameSrc,strlen(filenameSrc));
            write(1,"\n",1);
        } 
    }
    free(buf);  
    buf = NULL; 
	return;
}  

