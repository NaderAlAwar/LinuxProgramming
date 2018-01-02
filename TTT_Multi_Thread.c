#include <stdio.h> /*for printf, scanf*/
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
//pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
char checkWinner(char* state);
void printState(char* state, int turn);	
int minimax(char* board, int maxPlayer, char player, int depth){
	if(depth == 8){
	char winner = checkWinner(board);
	if(winner == player)
		return 9-depth;
	else if(winner == 'd')
		return 0;
	else
		return 	depth-9;
	}
	char winner = checkWinner(board);
	if(winner == player)
		return 9-depth;
	else if(winner == ' ' || winner == 'n'){}
	else
		return depth-9;
	if(maxPlayer == 1){
		int score = -10;
		int move = 0;
		for(int i = 0; i < 9; i++){
			if(board[i] == ' '){
				char* testBoard = (char*)malloc(9*sizeof(char));
				for(int j = 0; j < 9; j++)
					testBoard[j] = board[j];
				testBoard[i] = player;
				int testScore = score;
				if(player == 'X')
					testScore = minimax(testBoard, 0, 'O',depth+1);
				else
					testScore = minimax(testBoard, 0, 'X',depth+1);
				if(testScore > score)
					score = testScore;
				free(testBoard);
			}
		}
		return score;
	}
	else{
		int score = 10;
		int move = 0;
		for(int i = 0; i < 9; i++){
			if(board[i] = ' '){
				char* testBoard = malloc(9*sizeof(char));
				for(int j = 0; j < 9; j++)
					testBoard[j] = board[j];
				testBoard[i] = player;
				int testScore = score;
				if(player == 'X')
					testScore = minimax(testBoard, 1, 'X',depth+1);
				else
					testScore = minimax(testBoard, 1, 'O',depth+1);
				if(testScore < score)
					score = testScore;
				free(testBoard);
			}
		}
		return score;
	}
}
			
char currentPlayer = 'X';
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
	srand(time(NULL));	
	int ran = rand() % j;
	state[choices[ran]] = turn;
	free(choices);
}
void makeBetterDecision(char* state, char turn){
	if(state[4] == ' ')
		state[4] = turn;
	else{
	int j = 0;
	for(int i = 0; i < 9; i++){
		if(state[i] == ' ')
			j++;
	}
	int score = -10;
	int move = 0;
	for(int i = 0; i < 9; i++){
		if(state[i] == ' '){
			int testScore = minimax(state, 1, turn, 8 - j);
			if(testScore > score){
				score = testScore;
				move = i;
			}
		}
	}
	state[move] = turn;
	}
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
typedef struct{
	char* board;
	char* youAre;
} runArgs;

void *run(void * args){		//player: X or O, gameState: p(playing) or f (finish)
	runArgs *actualArgs = args;
	char* gameBoard = actualArgs->board;
	char* thPlayer = actualArgs->youAre;
	while(*thPlayer != 'F'){
		if(*thPlayer == currentPlayer){
//			pthread_mutex_lock(&mutex1);
			makeDecision(gameBoard, *thPlayer);
			currentPlayer = 'M';				//to transfer control back to main thread
//			pthread_mutex_unlock(&mutex1);
		}
	}		
}
int main(){
	char *gameBoard = malloc(9*sizeof(char));
	for(int i = 0; i < 9; i++)
		gameBoard[i] = ' ';
	char player1 = 'X';
	char player2 = 'O';	
	runArgs *passX = malloc(sizeof(runArgs));
	passX->board = gameBoard;
	passX->youAre = &player1;
	runArgs *passO = malloc(sizeof(runArgs));
	passO->board = gameBoard;
	passO->youAre = &player2;
	pthread_t playerX, playerO;
	pthread_create(&playerX, NULL, run, passX);	
	pthread_create(&playerO, NULL, run, passO);
	int turn = 0;
	while(currentPlayer != 'F'){
		if(currentPlayer == 'M'){	
			printState(gameBoard,turn);
			turn++;
			char winner = checkWinner(gameBoard);
			if(winner == 'n'){				//if game is still going on			
				currentPlayer = whoseTurn(gameBoard);
			}
			else if(winner == 'd'){
				currentPlayer = 3;
				printf("It's a draw");
				return 0;	
			}
			else{ 
				currentPlayer = 3;
				printf("The winner is %c", winner);
				return 0;
			}
		}	
}
pthread_join(playerX, NULL);
pthread_join(playerO, NULL);
free(gameBoard);
return 0;	   
}
