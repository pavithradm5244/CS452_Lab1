 /*
 * This code implements a simple shell program
 * It supports the internal shell command "exit", 
 * backgrounding processes with "&", input redirection
 * with "<" and output redirection with ">".
 * However, this is not complete.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>

//prototypes for void methods
void pipeIndex(char **args, int pipeNum, int pipeCollection[]);
void args_split(char **args, int start, int end, char **buf);

extern char **getlineShell();

/*
 * Handle exit signals from child processes
 */
void sig_handler(int signal) {
  int status;
  int result = wait(&status);

  printf("Wait returned %d\n", result);
}

/*
 * The main shell function
 */ 
main() {
  int i;
  char **args; 
  char **buf;	// buffer for sliced args
  char **command; 
  int result;
  int block;
  int output;
  int input;
  int append;
  int logicalAnd;
  char *output_filename;
  char *input_filename;
  char *append_filename;

  // Set up the signal handler
  //sigset(SIGCHLD, sig_handler);

  // Loop forever
  while(1) {

    // Print out the prompt and get the input
    printf("->");
    args = getlineShell();

    // No input, continue
    if(args[0] == NULL)
      continue;

    // Check for internal shell commands, such as exit
    if(internal_command(args))
      continue;
  
	// Check for pipes
	int pipeNum = pipeNumCounter(args);
	
	//printf("\npipenum is %d\n\n", pipeNum);

	if (pipeNum > 0) {
	
		// Check for an ampersand
		block = (ampersand(args) == 0);

		

		int fd[10][2];

		int pipeCollection[pipeNum+2];
		pipeIndex(args, pipeNum, pipeCollection);


		int p;
		
		for( i = 0; i < pipeNum+1; i++) {
			
			
			// get args for loop iteration
			args_split(args, pipeCollection[i], pipeCollection[i+1], buf);
			
			// create first pipe
			if (pipe(fd[i]) == -1) {
				printf("error creating pipe");
				break;
			}
		
			if (fork()==0) {
				//child
			
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
			
			//parent process
			if(i!=0) {
				close(fd[i-1][0]);
				close(fd[i-1][1]);
			}
		
			wait(NULL);
		
		}
		
		
	}
	else { 
		// do this if no pipes
		
		
		//Check for logicalAnd.
		command = malloc(30 * sizeof(char*));
		logicalAnd = and_command(args, command);
		
		// Check for an ampersand
		block = (ampersand(args) == 0);

		// Check for redirected input
		input = redirect_input(args, &input_filename);


		switch(input) {
		case -1:
			printf("Syntax error!\n");
			continue;
			break;
		case 0:
			break;
		case 1:
			printf("Redirecting input from: %s\n", input_filename);
			break;
		}

		// Check for append input
		append = append_output(args, &append_filename);

		switch(append) {
		case -1:
			printf("Syntax error!\n");
			continue;
			break;
		case 0:
			break;
		case 1:
			printf("Appending output to: %s\n", append_filename);
			break;
		}    


		// Check for redirected output
		output = redirect_output(args, &output_filename);

		switch(output) {
		case -1:
			printf("Syntax error!\n");
			continue;
			break;
		case 0:
			break;
		case 1:
			printf("Redirecting output to: %s\n", output_filename);
			break;
		}
		
		// Do the command
		do_command(args, block, logicalAnd, command,
	       input, input_filename, 
	       output, output_filename, append, append_filename);
	}
  }
}

/*
 * Handle exit signals from child processes
 */
/*void sig_handler(int signal) {
  int status;
  int result = wait(&status);
  printf("Wait returned %d\n", result);
}*/



//Logical And
int and_command(char **args, char **command){
	int a;
	int b;
	int c = 0;

	for(a = 1; args[a] != NULL; a++){
		if(args[a][0] == '&' && args[a-1][0] == '&'){   //if && found
			free(args[a-1]);
			args[a-1] = NULL;
			free(args[a]);
			args[a] = NULL;

			for(b = a+1; args[b] != NULL; b++){   //copy command after && into command array
        //printf(args[b]);
				command[c] = args[b];
				c++;
			}
			return 1;
		}
	}
	return 0;
}



/*
 * Check for ampersand as the last argument
 */
int ampersand(char **args) {
  int i;

  for(i = 1; args[i] != NULL; i++) ;

  if(args[i-1][0] == '&') {
    free(args[i-1]);
    args[i-1] = NULL;
    return 1;
  } else {
    return 0;
  }
  
  return 0;
}

/* 
 * Check for internal commands
 * Returns true if there is more to do, false otherwise 
 */
int internal_command(char **args) {
  if(strcmp(args[0], "exit") == 0) {
    exit(0);
  }
  
  //cd command
  if(strcmp(args[0], "cd") == 0) {
    if(args[1] == NULL){
      printf("Syntax error!\n");
    }
    else if(chdir(args[1]) < 0){
      printf("No file directory named %s exists\n", args[1]);
    }
    else{
      printf("Changing directory to: %s \n", args[1]);
      chdir(args[1]);
    }
    return 1;
  }

  return 0;
}

/* 
 * Do the command
 */

int do_command(char **args, int block, int logicalAnd, char **command,
	       int input, char *input_filename,
	       int output, char *output_filename, int append, char *append_filename) {
  
  int result;
  pid_t child_id;
  int status;
  pid_t parent_gid;
  
  //Get parent process group id
  parent_gid = getpgid(0);

  // Fork the child process
  child_id = fork();

  // Check for errors in fork()
  switch(child_id) {
  case EAGAIN:
    perror("Error EAGAIN: ");
    return;
  case ENOMEM:
    perror("Error ENOMEM: ");
    return;
  }

  if(child_id == 0) {

    // Set up redirection in the child process
    if(input)
      freopen(input_filename, "r", stdin);

    if(output)
      freopen(output_filename, "w+", stdout);

  
    if(append)
      freopen(append_filename, "a+", stdout);

    // Execute the command
   if(!block){
	printf("Executing background process with pid %d\n", child_id);
	setpgid(child_id, 0);
    }
    result = execvp(args[0], args);
    exit(-1);
  }

  // Wait for the child process to complete, if necessary
  if(block) {
    //printf("Waiting for child, pid = %d\n", child_id);
    result = waitpid(child_id, &status, 0);
  }else{
    tcsetpgrp(0, parent_gid);
    signal(SIGCHLD, SIG_IGN);
    result = waitpid(-1, &status, WNOHANG);
  }

  //If command has a logical And (&&)
  if(logicalAnd){
    //Set up to check for another &&
    char **command2;

    //Check if first argument is == false
    int dif;
    dif = strcmp(args[0], "false");

    //Check for 2nd &&, save to new command array
    command2 = malloc(30 * sizeof(char*));
    logicalAnd = and_command(command, command2);

    if(logicalAnd > 0){   //If 2d && found
      printf("%s\n", command2[0]);
      printf("%d\n", dif);
      if(result > 0 && (dif >0 || dif < 0)){   //if result executed and first arg isnt false
        //Do second command
        do_command(command, block, 0, command,
          input, input_filename, 
          output, output_filename, append, append_filename);
        //Do third command
        do_command(command2, block, 0, command,
          input, input_filename, 
          output, output_filename, append, append_filename);
      }
      
    } else {  //if only one &&
      if(result > 0 && (dif >0 || dif < 0)){
        //Do second command
        do_command(command, block, 0, command,
          input, input_filename, 
          output, output_filename, append, append_filename);
        }
    }
  }
}


/*
 * Check for input redirection
 */
int redirect_input(char **args, char **input_filename) {
  int i;
  int j;

  for(i = 0; args[i] != NULL; i++) {

    // Look for the <
    if(args[i][0] == '<') {
      free(args[i]);

      // Read the filename
      if(args[i+1] != NULL) {
	*input_filename = args[i+1];
      } else {
	return -1;
      }

      // Adjust the rest of the arguments in the array
      for(j = i; args[j-1] != NULL; j++) {
	args[j] = args[j+2];
      }

      return 1;
    }
  }

  return 0;
}

/*
 * Check for output redirection
 */
int redirect_output(char **args, char **output_filename) {
  int i;
  int j;

  for(i = 0; args[i] != NULL; i++) {

    // Look for the >
    if(args[i][0] == '>' && args[i+1][0] != '>') {
      free(args[i]);

      // Get the filename 
      if(args[i+1] != NULL) {
	*output_filename = args[i+1];
      } else {
	return -1;
      }

      // Adjust the rest of the arguments in the array
      for(j = i; args[j-1] != NULL; j++) {
	args[j] = args[j+2];
      }

      return 1;
    }
  }

  return 0;
}

int append_output(char **args, char **append_filename) {
	int i;
	int j;
	
	for(i = 0; args[i] != NULL; i++) {
		// Look for the >>
		// The parser doesn't parse the >> together
		if(args[i][0] == '>' && args[i+1][0] =='>') {
			free(args[i]);
			free(args[i+1]);

			// Get the filename
			if(args[i+2] != NULL) {
				*append_filename = args[i+2];
			} else {
				return -1;
			}
			printf("filename: %s\n", *append_filename);
			//Adjust the rest of the arguments in the array
			for(j = i; args[j-1] != NULL; j++) {
				args[j] = args[j+3];
			}
			return 1;
		}
	}

	return 0;
}



//this splits the string array into parts based on pipes
void args_split(char **args, int start, int end, char **buf)
{
    int i;
    int j = 0;
    for (i = start+1; i < end; i++) {
	//	printf("args index %d is %s\n",i, args[i]);
        buf[j] = args[i];
		j++;
    }
    buf[j] = NULL;
}


int pipeNumCounter(char **args) {
	int i, pipeNum = 0;
	for(i = 0; args[i] != NULL; i++) {
		if(args[i][0] == '|') {
			if(args[i][1] != '|') {
				pipeNum++;
			}
		}	
	}
	return pipeNum;
}

// modifies pipeCollection array with indices of the pipes
void pipeIndex(char **args, int pipeNum, int *pipeCollection) {
	// first index has to be -1
	pipeCollection[0] = -1;
	int k;
	int j = 1; // indexer for pipeCollection starts at 1 cuz index 0 is -1
	for(k = 0; args[k] != NULL; k++) {
		if(args[k][0] == '|') {
			if(args[k][1] != '|') {
				pipeCollection[j] = k;
				j++;
			}

			//printf("j is %d\n", j);
		}
	}
	
	
	pipeCollection[j] = k;
	
}

