#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

extern char **getlineShell();
void pipeIndex(char **args, int pipeNum, int pipeCollection[]);

main() {
  int i;
  char **args; 

  while(1) {
	args = getlineShell();
	
	for(i = 0; args[i] != NULL; i++) {
		printf("Argument %d: %s\n", i, args[i]);
    	}
	int fd[10][2];



//	execvp(args[0], args);
/*
	int pipeNum = pipeNumCounter(args);

	printf("pipenum is %d\n",pipeNum);

	int pipeCollection[pipeNum];

	pipeIndex(args, pipeNum, pipeCollection);
	int i;
	printf("pipenum in function is %d\n", pipeNum);
	for(i=0;pipeCollection[i]!=NULL;i++) {
		printf("pc index %d is %d\n", i, pipeCollection[i]);
	}
*/
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

//need indices of pipes first
char** arrSlice(char **args, int start, int end) {
	int sliceSize = end - start - 1;

//	char** sliced[sliceSize][];

	
	

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
void pipeIndex(char **args, int pipeNum, int pipeCollection[]) {
	
	int k;
	int j = 0; // indexer for pipeCollection
	for(k = 0; args[k] != NULL; k++) {
		printf("%d", k);
		if(args[k][0] == '|') {
			(pipeCollection)[j] = k;
			j++;
		}
	}
	
}




