#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>  
#include <string.h>

// see reference: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program

void lfcat()
{
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		//printf("Current working dir: %s\n", cwd);
		DIR *pDir;
		FILE *freOp;
		FILE *fp;
		int fd;
		char *fileName;
		char *fileAdd;
		char *buf;
		char *tempLine;
		size_t bufsize = 255;
		size_t charCount;
		buf = (char *)malloc(bufsize * sizeof(char));
		tempLine = (char *)malloc(bufsize * sizeof(char));
		if ((buf == NULL) || (tempLine == NULL))
		{
			perror("Unable to allocate buffer");
			return;
		}
		pDir = opendir(cwd);
		struct dirent *dp;
		freOp = freopen("../output.txt","w",stdout);
		// struct dirent *readdir(DIR *dirp);
		while ((dp = readdir(pDir)) != NULL){
			// see ref https://www.appsloveworld.com/c/100/356/while-loop-and-if-not-getting-along-with-readdir-in-c?expand_article=1
			// or see ref https://stackoverflow.com/questions/39651705/while-loop-and-if-not-getting-along-with-readdir-in-c
			// pos debug here by printing out file names...
			if(dp->d_name[0] != '.') {
			if (freOp != NULL){
				//fd = open(fileName,O_RDONLY);
				fileName = "File: ";
				write(1, fileName, strlen(fileName));
				fileName = dp->d_name;
				write(1, fileName, strlen(fileName));
				fileName = "\n";			
				write(1, fileName, strlen(fileName));
				//close(fd);
				//printf("%s%s\n", "File: ", dp->d_name);
				
				fp = fopen(dp->d_name, "r");
				//fd = open(fileName,O_RDONLY);
				if (fp == NULL) {
					exit(EXIT_FAILURE);
				} else {
					while((charCount = getline(&buf,&bufsize,fp)) != -1){
						printf("%s",buf);
						//strncpy(tempLine,buf,255);
						//strcpy(tempLine, buf);
						//write(fd, &buf, charCount);
						//printf("%zu characters read\n", charCount);
						//printf("%s%zd\n", "Line Len:", strlen(tempLine));
					
					}
				}
				fclose(fp);
			}
			

			//
			
			//printf("%s\n",dp->d_name);
			//printf("%s\n", "A test message - one line per file");
			
			
			
			//fclose(fp);
			//fclose(dp->d_name);

			
			//int fd = open(dp->d_name, O_RDONLY, 0644);
			//fp = fopen(dp->d_name, "r");
			/*
			if(fd < 0){
				perror("Error opening file!");
				return;
			} else {
				charCount = getline(&buf,&bufsize,stdin);
				printf("%zu characters read\n", charCount);
				printf("buf: %s\n", buf);
				while(charCount != -1){
					//stdout << buf << endl;
					write(1, &buf, charCount);
				}
				fclose(stdout);
			}
			*/
			printf("\n%s\n", "----------------------------------------------------------------------------------------------------");
			}

			//open the file for reading...

			//use getline to read the current file..
			//close the file with fclose(), potentially "null assigning your buffer/file descriptors"
			//write 80 "-" characters before repeating the loop 
    	}
	// pos put this in the loop, or remove it..
	//fclose(stdout);
	//free(buf);
	buf = NULL;
	closedir(pDir);
	//printf(">>>:");
		
	} else {
		// perror? potentially find alternative error
		perror("getcwd() error");
		return;
	}
	return;
/* High level functionality you need to implement: */

	/* Get the current directory with getcwd() */
	
	/* Open the dir using opendir() */
	
	/* use a while loop to read the dir with readdir()*/
	
		/* You can debug by printing out the filenames here */

		/* Option: use an if statement to skip any names that are not readable files (e.g. ".", "..", "main.c", "lab2.exe", "output.txt" */
			
			/* Open the file */
			
			/* Read in each line using getline() */
				/* Write the line to stdout */
			
			/* write 80 "-" characters to stdout */
			
			/* close the read file and free/null assign your line buffer */
	
	/*close the directory you were reading from using closedir() */
}
