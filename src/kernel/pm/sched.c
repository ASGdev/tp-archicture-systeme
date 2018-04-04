/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
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

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>
#include <nanvix/config.h>

//==================================ADDED BY THE SQUALE TEAM
enum state
{
	TODO, 		//The process has still some validity it can use (tab_prio->quantums_left > 0)
	DONE, 		//The process has used all of the validity it can use
	INVALID 	//The process in proc_table is null
};
typedef enum state state;


struct tab_prio
{
	int quantums_left[PROC_MAX]; 	// The number of quantums remaining for one process
	state validity[PROC_MAX];		// The state of a process
};
typedef struct tab_prio tab_prio;

PRIVATE struct tab_prio tab;

//Initiate the tab_prio tab with the processes in proctab
PUBLIC void init_tab_prio();

//Get the number of quantum according to the process priority
PUBLIC int translate_priority(struct process p);

//TODO shall be the yield function
PUBLIC void processing ();

//TODO -> check the return value if works correctly
PUBLIC int translate_priority(struct process p)
{
	int niceness = p.nice; 	//number between 0 and 39

	return (niceness/4)+1;	//The number of quantum is 1 + the rank in between niceness/4
							//we have chosen 4 for the size of quantum 
}


PUBLIC void init_tab_prio()
{
	/*	At a point this will be set up to 1 which means that 
	*	the following i in the loop will be null in the proctab
	*/
	int nextnull = 0; 

	for(int i = 0 ; i < PROC_MAX; i++)
	{
		if(!nextnull)
		{
			tab.quantums_left[i] = translate_priority(proctab[i]);
			tab.validity[i] = TODO;
			if(proctab[i].next == NULL)
				nextnull = 1;		
		}
		else{
			tab.validity[i] = INVALID;
		}
	}
}

PUBLIC int do_we_re_init ()
{
	
	for(int i = 0 ; i < PROC_MAX ;  i++)
	{
		if(tab.validity[i] == TODO)
			return 1;
		if(tab.validity[i] == INVALID)
			return 0;
	}
	return 0;
}


//================================== SQUALE TEAM OVER


/**
 * @brief Schedules a process to execution.
 * 
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;
}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	yield();
}

/**
 * @brief Resumes a process.
 * 
 * @param proc Process to be resumed.
 * 
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{	
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc);
}

/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	if(do_we_re_init())
		init_tab_prio();

	int t = 0;
	for (int i = 0 ; i < PROC_MAX; i++)
	{
		if(tab.validity[i] == TODO)
			t++;
		if(tab.validity[i] == INVALID)
			break;
	}

	int randomed = rand()*t;

	struct process* next = &(proctab[randomed]);

	/* Switch to next process. */
    next->priority = PRIO_USER;
    next->state = PROC_RUNNING;
    next->counter = PROC_QUANTUM;

    tab.quantums_left[randomed]--;
    if(tab.quantums_left[randomed] == 0){
    	tab.validity[randomed] = DONE;
    }


    switch_to(next);

}

/* RAND PART */
static unsigned _next = 1;

void set_rand(int seed){
	_next = seed;
}

int rand(void)
{
	_next = (_next * 1103515245) + 12345;
	return ((_next >> 16) & 0x7fff);
}
/**/

