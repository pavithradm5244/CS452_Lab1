#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

extern char **getlineShell();

main() {
  int i;
  char **args; 

  while(1) {
    args = getlineShell();



    int pipenum;
    pipenum = pipeCheck(args);

    printf("the number of pipes is %d\n", pipenum);

    myPipe(args, pipenum);
  }
}

// Check if there is a |
int pipeCheck(char **args) {
	int i = 0;
	int numPipes = 0;
	for(i = 1; args[i] != NULL; i++) {
		// this doesn't check the last index for pipes
		if(args[i-1][0] == '|') {
			numPipes++;
		}
	}
	return numPipes;
}



int myPipe(char **args, int numPipes) {
	int i = 0;
	pid_t pid;
	int index = pipeIndex(args);
	printf("index is %d\n", index);

//	if (index != -1) {
		char** left[index-1];
		char** right = {"more"};
		for (i = 0; i < index; i++) {
			printf("args %d is %s\n", i, args[i]);
			left[i] = args[i];
		}
//	}
	int j;

	int result;

	printf("executing left\n");
	execvp(left[0], left);	


	/*
	int fd[2];
	pipe(fd);


	pid = fork();

	if(pid == 0) {
		dup2(fd[0], 1);
		close(fd[1]);
		close(fd[0]);
		execvp(left[0], left);
		fprintf(stderr, "Failed to execute left '%s'\n", left);
		exit(1);
	}

	else {
		pid=fork();

		if(pid==0) {
			dup2(fd[1], 1);
			close(fd[0]);
			close(fd[1]);
			execvp(right[0], right);
			fprintf(stderr, "Failed to execute right '%s'\n", right);
			exit(1);
		}
		else {
			int status;
			close(fd[1]);
			close(fd[0]);
			waitpid(pid, &status, 0);
		}
	}


	Ignore stuff below this
=========================================
	
	int pipeNum = 0;
	int fd[2 * numPipes];
	
	// we will close the input of the file if its not 0, then we'll make the output into a pipe
	for(i = 0; i < (numPipes); i++){
        	if(pipe(fd + i*2) < 0) {
            		perror("couldn't pipe");
        	}
    	}
	
	int j = 0;
	
	while(args[j] != NULL) {
		pid = fork();
		if (pid == 0) {
		
			// This checks if the arg is the last one, if not redirects output
			if(args[j+1] != NULL){
               			if(dup2(fd[j + 1], 1) < 0){
                			perror("dup2 failed");
                		}
            		}
		
			//If its not the first command, redirects input
			if(j != 0) {
				if(dup2(fd[j-2], 0) < 0){
					perror(" dup2 failed on input");
				}
			}
		}
	}
*/
	
	
	
}

int pipeIndex(char** args) {
	int i;
	for( i = 0; args[i] != NULL; i++) {
		if(args[i][0] == '|') {
			return i;
		}
	}
	return -1;
}

