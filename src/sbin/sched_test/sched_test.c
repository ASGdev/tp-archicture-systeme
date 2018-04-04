/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2016-2016 Davidson Francis <davidsondfgl@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <nanvix/config.h>
#include <nanvix/klib.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


/* Test flags. */
#define EXTENDED (1 << 0)
#define FULL     (1 << 1)
#define VERBOSE  (1 << 2)

/* parameters */
#define N_PROC 5
const int niceness[N_PROC] = {1, 2, 3, 4};

// static void work(){
// 	int c;
	
// 	c = 0;
		
// 	/* Perform some computation. */
// 	for (int i = 0; i < 4096; i++)
// 	{
// 		int a = 1 + i;
// 		for (int b = 2; b < i; b++)
// 		{
// 			if ((i%b) == 0)
// 				a += b;
// 		}
// 		c += a;
// 	}
// }

static int do_test(){
	pid_t pid[N_PROC];
	
	for (int i = 0; i < N_PROC; i++)
	{
		printf("forking new child\n");
		pid[i] = fork();
	
		/* Failed to fork(). */
		if (pid[i] < 0)
			printf("Fork failed");
			return (-1);
		
		/* Child process. */
		else if (pid[i] == 0)
		{
			printf("Running new child with pid %d and niceness %d\n", getpid(), niceness[i]);
			nice(niceness[i]);
			//work();
			_exit(EXIT_SUCCESS);
		}
	}
	
	return (0);
}



/*============================================================================*
 *                                   main                                     *
 *============================================================================*/

/**
 * @brief System testing utility.
 */
int main()
{
	printf("running test\n");
	do_test();
	
	return (EXIT_SUCCESS);
}
