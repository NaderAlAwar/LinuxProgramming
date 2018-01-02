#include <sys/types.h> /* for fork*/
#include <unistd.h>/* for pipe, dup2, fork*/
#include <stdio.h> /*for printf, scanf*/
#include <sys/shm.h>/*for shmget and shmat*/
#include <sys/ipc.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
void printState(char* state, int turn){
	printf("turn %d:\n", turn);
	for(int i = 0; i < 9; i++){
		if( i == 2 || i == 5){
			printf(" %c\n", state[i]);
			printf("---+---+---\n");	
		}
		else if(i == 8){
			printf(" %c\n", state[i]);
		}
		else{
			printf(" %c |", state[i]);
		}
	}
}	
char whoseTurn(char* state){
	int countX = 0;
	int countO = 0;
	for(int i = 0; i < 9; i++){
		if(state[i] == 'X'){
			countX++;
		}
		else if(state[i] == 'O'){
			countO++;
		}
	}
	if(countX > countO)
		return 'O';
	else
		return 'X';
}

void makeDecision(char* state, char turn){
	sleep(4);
	int* choices = malloc(9*(sizeof(int)));
	int j = 0;
	for(int i = 0; i < 9; i++){
		if(state[i] == ' '){
			choices[j] = i;
			j++;
		}
	}
//	printf("remaining choices %d", j);
	srand(time(NULL));	
	int ran = rand() % j;
	state[choices[ran]] = turn;
	free(choices);
}
char checkWinner(char* state){
	if(state[0] == state[1] && state[1] == state[2] && state[0] != ' ')
		return state[0];
	if(state[3] == state[4] && state[4] == state[5] && state[3] != ' ')
		return state[3];
	if(state[6] == state[7] && state[7] == state[8] && state[6] != ' ')
		return state[6];
	if(state[0] == state[3] && state[3] == state[6] && state[0] != ' ')
		return state[0];
	if(state[1] == state[4] && state[4] == state[7] && state[1] != ' ')
		return state[1];
	if(state[2] == state[5] && state[5] == state[8] && state[2] != ' ')
		return state[2];
	if(state[0] == state[4] && state[4] == state[8] && state[4] != ' ')
		return state[0];
	if(state[2] == state[4] && state[4] == state[6] && state[2] != ' ')
		return state[2];
	for(int i = 0; i < 9; i++){
		if(state[i] == ' ')
			return 'n';
	}	
	return 'd';
}
int main(){
	struct timeval t1, t2, t3, t4;
	double elapsedTime;
	pid_t   child_a_pid;
 	pid_t   child_b_pid;	
	int ShmIDState;							//id of state of board
	char *ShmPTRState;						//pointer to state of board
	gettimeofday(&t1, NULL);
	ShmIDState = shmget(1234, 9*sizeof(char),IPC_CREAT | 0666);
	gettimeofday(&t2, NULL);
	elapsedTime = t2.tv_sec-t1.tv_sec;
	printf("time for shmget is %lf", elapsedTime);

	gettimeofday(&t3, NULL);
	ShmPTRState = (char*) shmat(ShmIDState, NULL, 0);
	
	gettimeofday(&t4, NULL);
	
	elapsedTime = t4.tv_sec-t3.tv_sec;
	printf("time for shmat is %lf", elapsedTime);
	for(int i = 0; i < 9; i++){
		ShmPTRState[i] = ' ';
	}
	int ShmIDTurn;							//id of whose turn it is
	int* ShmPTRTurn;						//pointer to whose turn it is
	ShmIDTurn = shmget(1235, sizeof(int), IPC_CREAT | 0666);
	ShmPTRTurn = (int*) shmat(ShmIDTurn, NULL, 0);
	*ShmPTRTurn = 0;						//0 means processA (X) and 1 means processB (O) and 2 means main process and 3 means finished
	if ((child_a_pid = fork()) < 0){
       		 /* FATAL: cannot fork child_a */
     		 return -5;
	}
	else if (child_a_pid == 0){
		while(*ShmPTRTurn < 3){
			if(*ShmPTRTurn == 0){
				//do stuff
				makeDecision(ShmPTRState,'X');	
				*ShmPTRTurn = 2;
			}	
		}
		return 0;
	}    
	else{
		if((child_b_pid = fork()) < 0){
			return -6;
		}
		else if(child_b_pid == 0){
			while(*ShmPTRTurn < 3){


				if(*ShmPTRTurn == 1){
					//do stuff
					makeDecision(ShmPTRState,'O');	
					*ShmPTRTurn = 2;
				}
			}
			return 0;			
		}
		else{
			int turn = 0;
			while(*ShmPTRTurn < 3){
				if(*ShmPTRTurn == 2){
					
					printState(ShmPTRState,turn);
					turn++;
					char winner = checkWinner(ShmPTRState);
					if(winner == 'n'){				//if game is still going on			
						char turn = whoseTurn(ShmPTRState);
						if(turn == 'X')
							*ShmPTRTurn = 0;
						else
							*ShmPTRTurn = 1;
					}
					else if(winner == 'd'){
						*ShmPTRTurn = 3;
						printf("It's a draw");
						return 0;	
					}
					else{ 
						*ShmPTRTurn = 3;
						printf("The winner is %c", winner);
						return 0;
					}
				}	
			}	
		}
	}	   
return 0;
}
