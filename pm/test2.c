#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


void compteur(int i, int value)
{
	for(int j = 0; j < i ; j++)
	{
		printf("%d", value);
	}
}


int main()
{
	int i = 1;
	for (int k = 0 ; k < 6 ; k++){
		if(i == -1) exit(-1);
		if(i != 0){//Pere
			i = fork();
		}
		else{//Fils
			compteur(50, k);
		}
	}
}