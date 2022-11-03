#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp
#include <sys/wait.h> // wait
#include <signal.h> // signal
#include <fcntl.h> // open
#define MAX_CMD_ARG 10
#define BUFSIZ 256

const char *prompt = "myshell> ";
char* cmdvector[MAX_CMD_ARG];
char  cmdline[BUFSIZ];

void fatal(char *str){
  perror(str);
  exit(1);
}

int makelist(char *s, const char *delimiters, char** list, int MAX_LIST){ 
  int i = 0;
  int numtokens = 0;
  char *snew = NULL;

  if( (s==NULL) || (delimiters==NULL) ) return -1;

  snew = s + strspn(s, delimiters); /* Skip delimiters */
  if( (list[numtokens]=strtok(snew, delimiters)) == NULL )
    return numtokens;
  
  numtokens = 1;
  
  while(1){
     if( (list[numtokens]=strtok(NULL, delimiters)) == NULL)
  break;
     if(numtokens == (MAX_LIST-1)) return -1;
     numtokens++;
  }

  return numtokens;
}

// SIGCHLD signal handler function
static void child_handler(int sig){ 
	int status;
	pid_t pid;
	while((pid = waitpid(-1, &status, WNOHANG)) > 0){}
}

// Function that prevents termination
void handler_func(int sig){
	printf("\n");
	return;
}

// stdin
int input_redirection(char **cmdvector){
	int i;
	int fd; // file descriptor
	
	for( i = 0; cmdvector[i] != NULL; i++)
	{
		if(!strcmp(cmdvector[i], "<")) // check for "<"
		{
			break;
		}
	}
	
	if(cmdvector[i]){
		if(!cmdvector[i + 1])
		{
			return -1;
		}
		else
		{
			// Open a file as read-only
			if((fd = open(cmdvector[i + 1], O_RDONLY)) == -1)
			{
				perror(cmdvector[i + 1]);
				return -1;
			}
		}
		
		dup2(fd, 0); // Redirection of standard input to file
		
		close(fd);
		
		// Pull the cmdvector forward
		for(i = i; cmdvector[i] != NULL; i++)
		{
			cmdvector[i] = cmdvector[i + 2];
		}
		
		cmdvector[i] = NULL;
	}
	
	return 0;
}

// stdout
int output_redirection(char **cmdvector){
	int i;
	int fd;
	for( i = 0; cmdvector[i] != NULL; i++)
	{
		if(!strcmp(cmdvector[i], ">")) // check for ">"
		{
			break;
		}
	}
	
	if(cmdvector[i]){
		if(!cmdvector[i + 1])
		{
			return -1;
		}
		else
		{
			// Open a file as read/write mode
			// Create a file if it doesn't exist
			// Permission mode is set to 0644
			if((fd = open(cmdvector[i + 1], O_RDWR | O_CREAT, 0644)) == -1)
			{
				perror(cmdvector[i + 1]);
				return -1;
			}
		}
		
		dup2(fd, 1); // Redirection of standard output to file
		
		close(fd);
		
		cmdvector[i] = NULL;
		cmdvector[i+1] = NULL;
		
		// Pull the cmdvector forward
		for(i = i; cmdvector[i] != NULL; i++)
		{
			cmdvector[i] = cmdvector[i + 2];
		}
		
		cmdvector[i] = NULL;
	}
	
	return 0;
}

int pipe_func(char **cmdvector){
	int k, i;
	pid_t pid1;
	int fd[2]; // Creates a pipe with file descriptors
	int pipe_check = 0; // 0 : no exist, 1 : exist
	int pipe_position; // if "|" exists
	char* cmdpipe1[10]; // Command in front of "|"
	char* cmdpipe2[10]; // Command at the back of "|"
	
	for(k = 0; cmdvector[k] != NULL; k++) // check for "|"
	{
		if(!strcmp(cmdvector[k], "|"))
		{
			pipe_check = 1; // exist
			pipe_position = k; // save position
		}
	}
	
	if(pipe_check == 0) // if "|" does no exist
		return 1;
	
	// Save the command before the pipe command in the cmdpipe1
	for(i = 0; i < pipe_position; i++)
	{
		cmdpipe1[i] = cmdvector[i];
	}
	cmdpipe1[i] = NULL;

	int index = 0;
	// Save the command after the pipe command in the cmdpipe2
	for(i = i+1; cmdvector[i] != NULL; i++){
		cmdpipe2[index] = cmdvector[i];
		index++;
	}	
	cmdpipe2[index] = NULL;
	
	pipe(fd); // system call to create pipe

	pid1 = fork();
	switch(pid1)
	{
		case -1 : perror("fork error"); break;
		case 0:
			input_redirection(cmdpipe1);
			dup2(fd[1], 1);
			close(fd[1]);
			close(fd[0]);
			execvp(cmdpipe1[0], cmdpipe1);
			exit(0);
		default:
			output_redirection(cmdpipe2);
			dup2(fd[0], 0);
			close(fd[1]);
			close(fd[0]);
			execvp(cmdpipe2[0], cmdpipe2);
			exit(0);
	}
}
			

int main(int argc, char**argv){
  int i=0;
  pid_t pid;
  signal(SIGCHLD, (void*)child_handler); // child process state change
  signal(SIGINT, handler_func); // ctrl + c
  signal(SIGQUIT, handler_func); // ctrl + \'
  signal(SIGTSTP, handler_func); // ctrl + Z
  while (1) {
    fputs(prompt, stdout);
    fgets(cmdline, BUFSIZ, stdin);
    cmdline[strlen(cmdline) -1] = '\0';
    
    int num_tokens = makelist(cmdline, " \t", cmdvector, MAX_CMD_ARG);

    if(!strcmp(cmdvector[0], "cd")) // chdir
    {
	    chdir(cmdvector[1]);
	    continue; // loop re-recurring
    }
    
    if(!strcmp(cmdline, "exit")) // exit
    {
	exit(1);
    }

    int where = 1; // foreground : 1, background : -1

    if(!strcmp(cmdvector[num_tokens-1], "&")) // background
    {
	    where = -1;
	    cmdvector[num_tokens-1] = NULL; // & <- NULL
    }
    
    switch(pid=fork()){
      case 0: // child process
      	if(where == 1) { // foreground
		// signal execution
 		signal(SIGQUIT, SIG_DFL);
        	signal(SIGINT, SIG_DFL);
        	signal(SIGTSTP, SIG_DFL);
	}
	if(where == -1){ // background
		// signal ignore 
		signal(SIGINT, SIG_IGN); 
		signal(SIGQUIT, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
	}
	pipe_func(cmdvector); // pipe
	input_redirection(cmdvector); // stdin
	output_redirection(cmdvector); // stdout	
        execvp(cmdvector[0], cmdvector); // exec
        fatal("main()");
      case -1:
        fatal("main()");
    }
    
    if(where == 1) // foreground - wait
	    wait(NULL);
  
  }
  return 0;
}


