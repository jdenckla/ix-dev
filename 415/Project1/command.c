#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "string_parser.h"

#include <dirent.h>
#include <string.h>

#include <stdlib.h>

// via https://www.includehelp.com/c-programs/check-a-given-filename-is-a-directory-or-not.aspx
// 0 is dir, 1 is file (according to documentation above)
int isDir(const char* fileName)
{
    struct stat path;
    stat(fileName, &path);
    return S_ISDIR(path.st_mode);
}

/*for the ls command*/
void listDir()
{
	char cwd[255];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		DIR *pDir;
		pDir = opendir(cwd);
		struct dirent *dp;
		while ((dp = readdir(pDir)) != NULL){
            write(1,dp->d_name,strlen(dp->d_name));
            //write(1,"\n",1);
            write(1," ",1);
    	}
        write(1,"\n",1);
	closedir(pDir);
	} 
	return;
} 

/*for the pwd command*/
void showCurrentDir() 
{
	char cwd[255];
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
        char *dirExists = "Directory already exists!";
        write(1,dirExists,strlen(dirExists));
        //write(1,dirName,strlen(dirName));
        //write(1,"' already exists!",17);
        write(1,"\n",1);
        return;
    }
	return;
} 

/*for the cd command*/
// find dir (?), then chane current to this
void changeDir(char *dirName)
{
    struct stat st = {0};
    if (stat(dirName, &st) != -1) {
        chdir(dirName);
        //write(1,"\n",1);
        //write(1,"dir changed",11);
        //write(1,"\n",1);
    } else {
        char *noDir = "Error! Could not find directory";
        write(1,noDir,strlen(noDir));
        /*
        write(1,"Error! ",7);
        write(1,dirName,strlen(dirName));
        write(1," does not exist!",16);
        */
        write(1,"\n",1);
        return;
    }
	return;
}  

/*for the cp command*/
// find file (open/close?) (or err/fail), find dest (?), cp file to dest
void copyFile(char *sourcePath, char *destinationPath)
{   
    //char cwd[255];
	//if (getcwd(cwd, sizeof(cwd)) != NULL) {
    size_t bufsize = 1024;
    char *buf;
    char *ptr;
    char *eof;
    size_t dataRead;
    int iter;
    int outFile;

    char *msg;
    msg = strdup(sourcePath);
    char *filenameSrc;
    char *filenameDest;
    filenameDest = strdup(destinationPath);
    command_line filename_buffer;
    filename_buffer = str_filler (msg, "/");
    filenameSrc = filename_buffer.command_list[filename_buffer.num_token - 1];
    char *err1;
    char *err2;
    char *err3;
    char *err4;
    char *err5;
    char *err6;
    //char *trailingDestCharacter = &filenameDest[strlen(filenameDest) - 1];

    
    err2 = "Error! Failed to open source file: ";
    int inFile = open(filenameSrc, O_RDONLY);
    if (inFile == -1) {
        write(1,err2,strlen(err2));
        write(1,filenameSrc,strlen(filenameSrc));
        write(1,"\n",1);
    } else {
        //changeDir(destinationPath);
        //changeDir(prev);
        // test the dirs above!!!
        buf = (char *)malloc(bufsize * sizeof(char));
        err6 = "Error! Unable to allocate buffer for file copy.";
        if (buf == NULL)
        {
            write(1,err6,strlen(err6));
            write(1,"\n",1);
        } else {
            /*
            if (strcmp("/", trailingDestCharacter)  != 0){
                strcat(filenameDest,"/");
                // mem issue, possibly toss this
            }
            */
            if (isDir(filenameDest) == 1) {
                strcat(filenameDest,"/");
                strcat(filenameDest,filenameSrc);
            }
            err3 = "Error! Failed to open/create dest file: ";
            outFile = open(filenameDest, O_WRONLY | O_CREAT, 0666);
            if (outFile == -1) {
                    write(1,err3,strlen(err3));
                    write(1,filenameSrc,strlen(filenameSrc));
                    write(1,"\n",1);
            } else {
                while(1) {
                    dataRead = read(inFile,buf,bufsize);
                    if (dataRead < 0) {
                        write(1,err3,strlen(err3));
                        write(1,filenameSrc,strlen(filenameSrc));
                        write(1,"\n",1);
                    } else if (dataRead == 0) {
                        break;
                    } 
                    ptr = buf;
                    eof = buf + dataRead;
                    while (ptr <= eof) {
                        dataRead = write(outFile, ptr, (size_t)(eof - ptr));
                        if (dataRead <= 0) {
                            break; 
                        } else {
                            ptr += dataRead;
                        }
                    }
                }
            }

            int r;
            r = chmod(filenameDest, S_IRWXU );
            err5 = "Error! Failed to modify permissions on dest file: ";
            if (r != 0) {
                write(1,err5,strlen(err5));
                write(1,filenameDest,strlen(filenameDest));
                write(1,"\n",1);
            }
        }
    }

    free_command_line (&filename_buffer);
	memset (&filename_buffer, 0, 0);
    //free(filenameSrc);
    free(filenameDest);
    free(msg);
    free(buf);  
    close(outFile);
    close(inFile);
    
    return;  
}  

/*for the mv command*/
// pos perform copyFile command, then deleteFile command
void moveFile(char *sourcePath, char *destinationPath)
{
	copyFile(sourcePath,destinationPath);
    unlink(sourcePath);
	return;
}  

/*for the rm command*/
// find file (open/close?) (or err/fail), delete it
void deleteFile(char *filename)
{
    char *filenameSrc;
    filenameSrc = strdup(filename);
    char *err1;
    err1 = "Error! Failed to find source file: ";
    int fd = open(filenameSrc, O_RDONLY);
    if (fd == -1) {
        write(1,err1,strlen(err1));
        write(1,filenameSrc,strlen(filenameSrc));
        write(1,"\n",1);
    } else {
        free(filenameSrc);
        unlink(filenameSrc);
    }
	return;
    // should be able to navigate to directory as well - consider token like copyFile above
}  



/*for the cat command*/
void displayFile(char *filename)
{
	size_t bufsize = 1024;
    char *buf;
    char *ptr;
    char *eof;
    size_t dataRead;
    int iter;

    char *filenameSrc;
    filenameSrc = strdup(filename);
    char *err2;
    char *err3;
    char *err4;
    char *err6;
    err2 = "Error! Failed to open source file: ";
    int inFile = open(filenameSrc, O_RDONLY);
    if (inFile == -1) {
        write(1,err2,strlen(err2));
        write(1,filenameSrc,strlen(filenameSrc));
        write(1,"\n",1);
    } else {
        if (inFile == -1) {
                write(1,err2,strlen(err2));
                write(1,filenameSrc,strlen(filenameSrc));
                write(1,"\n",1);
        }
        buf = (char *)malloc(bufsize * sizeof(char));
        if (buf == NULL)
        {
            write(1,err6,strlen(err6));
            write(1,"\n",1);
        } else {
            err3 = "Error! Failed to read source file";
            while(1) {
                dataRead = read(inFile,buf,bufsize);
                if (dataRead < 0) {
                    write(1,err3,strlen(err3));
                    write(1,filenameSrc,strlen(filenameSrc));
                    write(1,"\n",1);
                } else if (dataRead == 0) {
                    break;
                } 
                ptr = buf;
                eof = buf + dataRead;
                while (ptr <= eof) {
                    dataRead = write(1, ptr, (size_t)(eof - ptr));
                    if (dataRead <= 0) {
                        break; 
                    } else {
                        ptr += dataRead;
                    }
                }
            }
        }
    }
    write(1,"\n",1);
    free(buf);  
    free(filenameSrc);
    close(inFile);
	return;
}  

