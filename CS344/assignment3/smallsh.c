#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/wait.h> 
#include <signal.h>
#include <fcntl.h> 


#define MAX_CHAR 2048
#define MAX_ARGS 512


/*typedef struct process{

    char* input;
    
    char* output;

    char* command;

    char arguments[MAX_ARGS];

    bool checkbg;

	bool redirect;


} usercmd;*/


int fgvar = 1;



void SIGTSTP_Handler(int sigNo){ //foreground mode toggle function
	
	if(fgvar == 0){ //is off, will turn on

		fgvar = 1;

		char* errormsg = " Exiting foreground mode \n\n";

		write(1, errormsg, strlen(errormsg));

		fflush(stdout);

	}
	
	else{ //is on, will turn off

		fgvar = 0;

		char* on_message = "\n: Entering foreground mode \n";

		write(1, on_message, strlen(on_message));

		fflush(stdout);

	}

	return;

}




void cd(char * path){ //verifies if path exists or not
	
	if(chdir(path)){ //does not
		
		fprintf(stderr, "ERROR with %s \n", path);

		perror("");

		fflush(stderr);

		return;

	}

	else{ //does

		chdir(path);

		return;

	}

}




void userExit(){ //user exit function

	exit(0);

	return;

}




void cd_check(char ** childarr, int childarrIdx){

	if (childarrIdx == 1){
		
		cd(getenv("HOME"));

	}

	else{
		
		cd(childarr[1]);

	}

	return;

}



/*void makeNULL(usercmd *currCmd){

    for(int x = 0; x < bgprcs; x++){

        currCmd->arguments[x] = NULL;
    }

}
*/





int returnStatus(int pid_stat){  //gets pid status, function returns exit status or termination signal

	int status = 0;

	if (WIFEXITED(pid_stat)){

		printf("Exit status is: %d \n", WEXITSTATUS(pid_stat));

		fflush(stdout);

	}

	else{

		printf("Terminated by signal: %d \n", WTERMSIG(pid_stat));

		fflush(stdout);

	}

	return status;

}




/*void removeline(usercmd *currCmd){ //remove new line character from arguments list

    for(int x = 0; x < strlen(currCmd->arguments); ++x){

        if(currCmd->arguments[x] == "\n"){

            currCmd->arguments[x] = "\0";

            break;

        }
    }

}*/




void execute(struct sigaction sigint_handler, char ** childarr, int childarrIdx, int * pid_stat, char * redirectin, char * redirectout, int isbg){
	
	int result;
	
	pid_t temppid = -1655;

	pid_t currpid = -1655;     //random pid values (numbers of my fav f1 drivers)

	pid_t closedpid = -1655;

	temppid = fork();

	
	switch(temppid){
	
		case -1: //error

		perror("ERROR could not fork()");

		exit(1);

		break;

	
		case 0: //child

		if(isbg == 0){ //checks for the bg flag
			
			sigint_handler.sa_handler = SIG_DFL; //reset handler, terminating child

			sigaction(SIGINT, &sigint_handler, NULL);

		}


		
		if(strcmp(redirectout, "") != 0){ //check for redirection
	
			int file_out = open(redirectout, O_WRONLY | O_CREAT | O_TRUNC, 0666);

			if(file_out == -1){
				
				fprintf(stdout, "%s: no such file or directory \n", childarr[0]);

				fflush(stdout);

				exit(1);

			}


			result = dup2(file_out, 1);

			if(result == -1){

				perror("ERROR DUP2");

				fflush(stderr);

				exit(2);

			}

			fcntl(file_out, F_SETFD, FD_CLOEXEC); 

		}

		if(strcmp(redirectin, "") != 0){  //check for parsed redirection on the infile

			int file_in = open(redirectin, O_RDONLY);

			if(file_in == -1) {

				fprintf(stdout, "%s: no such file or directory \n", childarr[0]);

				fflush(stdout);

				exit(1);

			}

			result = dup2(file_in, 0);

			if(result == -1){

				perror("ERROR DUP2");

				fflush(stderr);

				exit(2);

			}

			fcntl(file_in, F_SETFD, FD_CLOEXEC); 

		}

		if(isbg == 1){	
			
			if(strcmp(redirectout, "") == 0){ //default to redirecting to /dev/null

				int file_out = open("/dev/null", O_WRONLY);

				if(file_out == -1){

					fprintf(stderr, "ERROR, cannot redirect to /dev/null \n");

					perror("");

					fflush(stderr);

					exit(1);

				}

				result = dup2(file_out, 1);

				if(result == -1){

					perror("ERROR DUP2");

					fflush(stderr);

					exit(2);

				}

				fcntl(file_out, F_SETFD, FD_CLOEXEC); //close file

			}
		
			if(strcmp(redirectin, "") == 0){  //default to redirecting to /dev/null

				int file_in = open("/dev/null", O_RDONLY); //open for read only

				if(file_in == -1){

					fprintf(stderr, "ERROR, cannot redirect to /dev/null \n");

					perror("");

					fflush(stderr);

					exit(1);

				}

				result = dup2(file_in, 1);

				if(result == -1){

					perror("ERROR DUP2");

					fflush(stderr);

					exit(2);

				}

				fcntl(file_in, F_SETFD, FD_CLOEXEC);

			}

		}
	
		childarr[childarrIdx] = NULL;

	
		if(execvp(childarr[0], (char **)childarr)){

			fprintf(stdout, "%s: no such file or directory \n", childarr[0]);

			fflush(stdout);

			fflush(stdin);					//flush all of the std streams

			fflush(stderr);

			exit(2);

		}

		exit(0);

		break;

		
		default: //parent

		if(isbg && fgvar){

			printf("Background pid is : %d \n", temppid);

			fflush(stdout);

		}

		else{ //launch in foreground
		
			currpid = waitpid(temppid, pid_stat, 0); //blocking wait so that it can run in the foregroubd like it is supposed t

		}

	}

	while((closedpid = waitpid(-1, pid_stat, WNOHANG)) > 0){

		printf("Background pid %d is done: ", closedpid);

		fflush(stdout);

		returnStatus(*pid_stat);

	}

}




int main() {

	char redirectout[MAX_CHAR]; 

	char redirectin[MAX_CHAR]; 		//buffers

	char *childarr[2048]; 


	struct sigaction sigint_handler = { { 0 } }; 
													//signal handler initialization
	struct sigaction sigtstp_handler = { { 0 } }; 


	int isbg; //bg/fg indicator

	int childarrIdx = 0; 

	int randval = 0; 


	sigint_handler.sa_handler = SIG_IGN;		//handlers for the program to ignore ctrl z and ctrl c

	sigfillset(&sigint_handler.sa_mask);


	sigint_handler.sa_flags = SA_RESTART;

	sigaction(SIGINT, &sigint_handler, NULL);


	sigtstp_handler.sa_handler = SIGTSTP_Handler;

	sigfillset(&sigtstp_handler.sa_mask);


	sigtstp_handler.sa_flags = SA_RESTART;

	sigaction(SIGTSTP, &sigtstp_handler, NULL);


	while(1){
		
		for(int i = 0; i < childarrIdx; ++i){
			
			if(childarr[i] != NULL){

				free(childarr[i]);

			}
		}

		char userdata[MAX_CHAR];

		char expandPID[30];

		char * tok; 

		childarrIdx = 0; 

		isbg = 0; 

		
		userdata[0] = '\0';

		expandPID[0] = '\0';
										//makes the end of arrays null
		redirectin[0] = '\0';

		redirectout[0] = '\0';

		
		printf(": "); //user prompt

		fflush(stdout);


		fgets(userdata, 300, stdin); //gets user data

		while (( userdata == NULL || userdata[0] == '#' || userdata[0] == '\0')){//blank line/comment check conditionals

			printf(": ");

			fflush(stdout);

			fgets(userdata, 300, stdin); 

		}

		tok = strtok(userdata, " \n");

		
		while(tok != NULL){ //redirection check

			if(strcmp(tok, ">") == 0){

				tok = strtok(NULL, " \n");

				strcpy(redirectout, tok);

			}
		
			else if (strcmp(tok, "<") == 0){

				tok = strtok(NULL, " \n");

				strcpy(redirectin, tok);

			}
		
			else if (strcmp(tok, "$$") == 0){

				sprintf(expandPID, "%d", getpid());

				childarr[childarrIdx] = strdup(expandPID);

				childarrIdx += 1;

			}

			else{
			
				childarr[childarrIdx] = strdup(tok);

			
				char temp[MAX_CHAR]; //temporary buffer array

				
				if (strlen(childarr[childarrIdx]) >= 2){

					if(childarr[childarrIdx][strlen(childarr[childarrIdx]) - 2] == '$' && childarr[childarrIdx][strlen(childarr[childarrIdx]) - 1] == '$'){
					
						tok[strlen(tok) - 2] = '\0';

						free(childarr[childarrIdx]);
						
						snprintf(temp, 300, "%s%d", tok, getpid());

						childarr[childarrIdx] = strdup(temp); 	

					}
				}

				childarrIdx += 1;

			}
			
			tok = strtok(NULL, " \n"); //reset token

		}

		
		if((strcmp(childarr[childarrIdx - 1], "&")) == 0){

			isbg = 1;

			childarr[childarrIdx - 1] = '\0';

			childarrIdx -= 1;

		}
		
		else{

			isbg = 0;

		}
		


		
		if((strcmp(childarr[0], "exit") == 0) ||(strcmp(childarr[0], "status") == 0) || (strcmp(childarr[0], "cd") == 0)){ //if/else if conditionals for checking for built in functions before code is executed

			if((strcmp(childarr[0], "exit") == 0)){

				userExit();

			}

			else if((strcmp(childarr[0], "status") == 0)){

				returnStatus(randval);

			}

			else if ((strcmp(childarr[0], "cd") == 0)){

				cd_check(childarr, childarrIdx);

			}

		}

		else{
			
			execute(sigint_handler, childarr, childarrIdx, &randval, redirectin, redirectout, isbg); //run command procession/execution code

		}
	}

	return 0;

}
