#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "functionheader.h"

int timeTermination = 100;
char logfile[30];
int noOfProcesses = 18;
int shmid;
holder *shmPtr;
pid_t *pids;

void help(){
	printf("HELP MENU:  \n\n");
}

int checkTime(int n){
	if(n > 100){
		printf("Will default to time 100s.\n");
		return 100;
	} else {
		return n;	
	}
}

//deallocates and frees everything
void cleanAll(){
	shmdt(shmPtr);
	shmctl(shmid, IPC_RMID, NULL);
	exit(0);
}

void ctrlC(){
	printf("Process terminate. CTRL + C caught.\n");
	cleanAll();
	exit(0);
}

void timesUp(){
	printf("The time given is up Process will terminate.\n");
	cleanAll();
	exit(0);
}

void resetPid(pid_t p){
	int i;
	for(i = 0; i < noOfProcesses; i++){
		if(pids[i] == p){
			pids[i] = 0;
			break;
		}
	}
}

//referred to stackoverflow
void mySigchldHandler(int sig){
	pid_t pid;
	while((pid = waitpid(-1, 0, WNOHANG)) != -1){
		resetPid(pid);
	}
}
int main(int argc, char *argv[]){

	int z;			//holds the lenght of logfile name entered
	char c;			//used for getopt
	key_t shmKey;

	signal(SIGALRM, timesUp);
	signal(SIGINT, ctrlC);
	signal(SIGKILL, cleanAll);

	//referred from stackoverflow
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = mySigchldHandler;
	sigaction(SIGCHLD, &sa, NULL);

	alarm(timeTermination);

	/////////////////////////////////////////////////////////////////
	//GETOPT
	
	while((c = getopt(argc, argv, "hs:l:")) != -1){
		switch(c){
			case 'h':
				help();
				return 0;
				break;
			case 's':
				timeTermination = checkTime(atoi(optarg));
				printf("Processes will terminate in %d sec(s)\n", timeTermination);
				alarm(timeTermination);
				break;

			case 'l':
				z = strlen(optarg);
				if(z < 30){
					strcpy(logfile, optarg);
					printf("logfile name: %s\n", logfile);

				} else {
					strcpy(logfile, "logfile.txt");
					printf("logfile name: %s\n", logfile);
				}
				break;

			default:
				errno = 3;
				perror("Please refer to -h help for proper use of the program.\n");
				exit(0);
		}
	}

	shmKey = ftok("./README.md", 'a');
	shmid = shmget(shmKey, (sizeof(processControlBlock) * MAX) + sizeof (holder), IPC_CREAT | 0666);
	if(shmid < 0){
		perror("oss: Error: shmget error, creation failure.\n");
		cleanAll();
		exit(1);
	}

	shmPtr = (holder *)shmat(shmid, NULL, 0);
	if(shmPtr == (holder *) -1){
		perror("oss: Error: shmget error, creation failure.\n");
		cleanAll();
		exit(1);
	}

	cleanAll();
	return 0;


///////////////////////////////END OF MAIN ////////////////////////////////////////////////////////	
}
