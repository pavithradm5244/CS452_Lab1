#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

extern char **getlineShell();
void pipeIndex(char **args, int pipeNum, int pipeCollection[]);
void args_split(char **args, int start, int end, char **buf);


main() {
  int i;
  char **args; 
  char **buf;

  while(1) {
	args = getlineShell();
	
	for(i = 0; args[i] != NULL; i++) {
		printf("Argument %d: %s\n", i, args[i]);
    }

	int fd[10][2];

	int pipeNum = pipeNumCounter(args);
	
	printf("\npipenum is %d\n\n", pipeNum);

	int pipeCollection[pipeNum+2];
	pipeCollection[0] = -1;

	// this changes pipecollection buffer
	pipeIndex(args, pipeNum, pipeCollection);

	int p;
		
	if (pipeNum > 0) {	
	// if there is one pipe there would need to be 2 forked processes
	for( i = 0; i < pipeNum+1; i++) {
		
		// create first pipe
		if (pipe(fd[i]) == -1) {
			printf("error creating pipe");
			return 1;
		}
		
		// get args for loop iteration
		args_split(args, pipeCollection[i], pipeCollection[i+1], &buf[0]);
		
		
		
			
		if (fork()==0) {
			//child
			
			printf("\niteration %d\n", i);
			
			printf("pipecol index %d and %d is %d and %d\n",i, i+1, pipeCollection[i], pipeCollection[i+1]);

		
			for (p=0; buf[p]!=NULL; p++) {
				printf("buf index %d is %s\n", p, buf[p]);
			}
			
			if(i!=pipeNum) {
				dup2(fd[i][1], 1);
				close(fd[i][0]);
				close(fd[i][1]);
			}
			
			if (i!=0) {
				dup2(fd[i-1][0], STDIN_FILENO);
				close(fd[i-1][0]);
				close(fd[i-1][1]);
			}
			
			execvp(buf[0],buf);
			
		}
		
		//parent
		if(i!=0) {
			close(fd[i-1][0]);
			close(fd[i-1][1]);
		}
		
		wait(NULL);
	}
	

	} else {
		execvp(args[0],args);
	}

  }
}

/*
//this is unused right now
//this finds the max length of a column in a 2d array so that we dont waste memory when
//we allocate a buffer in the future
//probably unneeded but... who knows
int maxSize(char **args) {
	int max = 0;
	int curr, i;

	for(i=0; args[i]!=NULL; i++) {
		curr = ((int) sizeof(args[i]) / sizeof(args[i][0]));	//get size of a column in the 2d array
		if(curr > max) {
			max = curr;
		}
	}

	return max;
}
*/

//this splits the string array into parts based on pipes
//needs a fencepost for the first and last slice
void args_split(char **args, int start, int end, char **buf)
{
    int i;
    int j = 0;
    for (i = start+1; i < end; i++) {
	//	printf("lalala %s\n", args[i]);
        buf[j] = args[i];
		j++;
    }
    buf[j] = NULL;
}

int pipeNumCounter(char **args) {
	int i, pipeNum = 0;
	for(i = 0; args[i] != NULL; i++) {
		if(args[i][0] == '|') {
			pipeNum++;
		}	
	}
	return pipeNum;
}

// modifies pipenumc array with indices the indexes of the pipes
void pipeIndex(char **args, int pipeNum, int *pipeCollection) {
	
	int k;
	int j = 1; // indexer for pipeCollection starts at 1 cuz index 0 is -1
	for(k = 0; args[k] != NULL; k++) {
		if(args[k][0] == '|') {
			pipeCollection[j] = k;
			j++;
			
			//printf("j is %d\n", j);
		}
	}
	
	
	pipeCollection[j] = k;
	
}




