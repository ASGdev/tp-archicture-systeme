                struct process *p;    /* Working process.     */
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
        for (p = FIRST_PROC; p <= LAST_PROC; p++)
        {
                /* Skip non-ready process. */
                if (p->state != PROC_READY)
                        continue;

                /*
                 * Process with higher
                 * waiting time found.
                 */
                if (p->counter > next->counter)
                {
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

