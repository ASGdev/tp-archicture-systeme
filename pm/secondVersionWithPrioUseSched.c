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

#include <stdlib.h>
//==================================ADDED BY THE SQUALE TEAM
#define PRIOR_FACTOR 10 //A more prior process will run PRIOR_FACTOR time more often than a less one.

/*Return a priority value compute by adding oldness and niceness.
** n is between 20 and 60
**	Value take between 1 and 100
** The oldness will be include between 1 and 1000. When a proc will be age more than 750,
** his value of priority will be the higher possible.
** Change this function in order to set a different scheduling between nicess and oldness
*/
PUBLIC int translate_priority(int n, int o)
{
	if ((o) >= 750)
		return 100;
	else
		return n;
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
	struct process *p;	/* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 1, sndsig(p, SIGALRM);
	}

	/* Choose a process to run next. */
	next = IDLE;
	int nextprio = 0;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		nextprio = translate_priority(next->nice, next->counter); //avoid repeat calc for stats
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		/*
				* consider that a more prior process will run PRIOR_FACTOR time before a less one
				* The more PRIOR_FACTOR is high, the more prior process will be schedule.
				*/
		if (p->priority > next->priority && nextprio < PRIOR_FACTOR)
		{
			next->counter++;
			next = p;
		}
		else if (translate_priority(p->nice, p->counter) > nextprio)
		{
			/*
						*Computing the most prior process
						* and age the other if it's more prior than the actual selected.
						* In fact, it's a FIFO with a niceness and oldness sched : when two proc have the same
						* prio, the first pick is keep.
					*/

			next->counter++;
			next = p;
		}

		/*
                 * Increment waiting
                 * time of process.
                 */
		else
			p->counter++;
	}

	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	switch_to(next);
}
