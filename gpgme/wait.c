/* wait.c 
 *	Copyright (C) 2000 Werner Koch (dd9jn)
 *
 * This file is part of GPGME.
 *
 * GPGME is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GPGME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include "syshdr.h"

#include "util.h"
#include "context.h"
#include "ops.h"
#include "wait.h"
#include "io.h"

/* Fixme: implement the following stuff to make the code MT safe.
 * To avoid the need to link against a specific threads lib, such
 * an implementation should require the caller to register a function
 * which does this task.
 * enter_crit() and leave_crit() are used to embrace an area of code
 * which should be executed only by one thread at a time.
 * lock_xxxx() and unlock_xxxx()  protect access to an data object.
 *  */
#define enter_crit()    do { } while (0)
#define leave_crit()    do { } while (0)
#define lock_table()    do { } while (0)
#define unlock_table()  do { } while (0)


struct wait_item_s {
    volatile int active;
    int (*handler)(void*,int,int);
    void *handler_value;
    int pid;
    int inbound;       /* this is an inbound data handler fd */
    GpgmeCtx ctx;
};

static int fd_table_size;
static struct io_select_fd_s *fd_table;

static void (*idle_function) (void);

static int do_select ( void );
static void run_idle (void);


static struct wait_item_s *
queue_item_from_context ( GpgmeCtx ctx )
{
    struct wait_item_s *q;
    int i;

    for (i=0; i < fd_table_size; i++ ) {
        if ( fd_table[i].fd != -1 && (q=fd_table[i].opaque) && q->ctx == ctx )
            return q;
    }
    return NULL;
}


static int
count_active_and_thawed_fds ( int pid )
{
    struct wait_item_s *q;
    int i, count = 0;
    
    for (i=0; i < fd_table_size; i++ ) {
        if ( fd_table[i].fd != -1 && (q=fd_table[i].opaque)
             && q->active && !fd_table[i].frozen && q->pid == pid  ) 
            count++;
    }
    return count;
}

/* remove the given process from the queue */
/* FIXME: We should do this on demand from rungpg.c */
static void
remove_process ( int pid )
{
    struct wait_item_s *q;
    int i;

    for (i=0; i < fd_table_size; i++ ) {
        if (fd_table[i].fd != -1 && (q=fd_table[i].opaque) && q->pid == pid ) {
            xfree (q);
            fd_table[i].opaque = NULL;
            fd_table[i].fd = -1;
        }
    }
}



/**
 * gpgme_wait:
 * @c: 
 * @hang: 
 * 
 * Wait for a finished request, if @c is given the function does only
 * wait on a finsihed request for that context, otherwise it will return
 * on any request.  When @hang is true the function will wait, otherwise
 * it will return immediately when there is no pending finished request.
 * 
 * Return value: Context of the finished request or NULL if @hang is false
 *  and no (or the given) request has finished.
 **/
GpgmeCtx 
gpgme_wait ( GpgmeCtx c, int hang ) 
{
    return _gpgme_wait_on_condition ( c, hang, NULL );
}

GpgmeCtx 
_gpgme_wait_on_condition ( GpgmeCtx c, int hang, volatile int *cond )
{
    struct wait_item_s *q;

    do {
        int did_work = do_select();

        if ( cond && *cond )
            hang = 0;

        if ( !did_work ) {
            /* We did no read/write - see whether the process is still
             * alive */
            assert (c); /* !c is not yet implemented */
            q = queue_item_from_context ( c );
            if (q) {
                if ( !count_active_and_thawed_fds (q->pid) ) {
                    remove_process (q->pid);
                    hang = 0;
                }
            }
            else
                hang = 0;
        }
        if (hang)
            run_idle ();
    } while (hang && !c->cancel );
    c->cancel = 0; /* fixme: fix all functions, to return a cancel error */
    return c;
}



/*
 * We use this function to do the select stuff for all running
 * gpgs.  A future version might provide a facility to delegate
 * those selects to the GDK select stuff.
 * This function must be called only by one thread!!
 * Returns: 0 = nothing to run
 *          1 = did run something 
 */

static int
do_select ( void )
{
    struct wait_item_s *q;
    int i, n;
    int any=0;
    
    n = _gpgme_io_select ( fd_table, fd_table_size );
    if ( n <= 0 ) 
        return 0; /* error or timeout */

    for (i=0; i < fd_table_size && n; i++ ) {
        if ( fd_table[i].fd != -1 && fd_table[i].signaled 
             && !fd_table[i].frozen ) {
            q = fd_table[i].opaque;
            assert (n);
            n--;
            if ( q->active )
                any = 1;
            if ( q->active && q->handler (q->handler_value,
                                          q->pid, fd_table[i].fd ) ) {
                DEBUG1 ("setting fd %d inactive", fd_table[i].fd );
                q->active = 0;
                fd_table[i].for_read = 0;
                fd_table[i].for_write = 0;
                fd_table[i].fd = -1;
            }
        }
    }
    
    return any;
}



/* 
 * called by rungpg.c to register something for select()
 */
GpgmeError
_gpgme_register_pipe_handler ( void *opaque, 
                              int (*handler)(void*,int,int),
                              void *handler_value,
                              int pid, int fd, int inbound )
{
    GpgmeCtx ctx = opaque;
    struct wait_item_s *q;
    int i;

    assert (opaque);
    assert (handler);
    
    q = xtrycalloc ( 1, sizeof *q );
    if ( !q )
        return mk_error (Out_Of_Core);
    q->inbound = inbound;
    q->handler = handler;
    q->handler_value = handler_value;
    q->pid = pid;
    q->ctx = ctx;
    q->active = 1;

    lock_table ();
 again:  
    for (i=0; i < fd_table_size; i++ ) {
        if ( fd_table[i].fd == -1 ) {
            fd_table[i].fd = fd;
            fd_table[i].for_read = inbound;    
            fd_table[i].for_write = !inbound;    
            fd_table[i].signaled = 0;
            fd_table[i].frozen = 0;
            fd_table[i].opaque = q;
            unlock_table ();
            return 0;
        }
    }
    if ( fd_table_size < 50 ) {
        /* FIXME: We have to wait until there are no other readers of the 
         * table, i.e that the io_select is not active in another thread */
        struct io_select_fd_s *tmp;

        tmp = xtryrealloc ( fd_table, (fd_table_size + 10) * sizeof *tmp );
        if ( tmp ) {
            for (i=0; i < 10; i++ )
                tmp[fd_table_size+i].fd = -1;
            fd_table_size += i;
            fd_table = tmp;
            goto again;
        }
    }

    unlock_table ();
    xfree (q);
    return mk_error (Too_Many_Procs);
}


void
_gpgme_freeze_fd ( int fd )
{
    int i;

    lock_table ();
    for (i=0; i < fd_table_size; i++ ) {
        if ( fd_table[i].fd == fd ) {
            fd_table[i].frozen = 1;
            DEBUG1 ("fd %d frozen", fd );
            break;
        }
    }
    unlock_table ();
}

void
_gpgme_thaw_fd ( int fd )
{
    int i;

    lock_table ();
    for (i=0; i < fd_table_size; i++ ) {
        if ( fd_table[i].fd == fd ) {
            fd_table[i].frozen = 0;
            DEBUG1 ("fd %d thawed", fd );
            break;
        }
    }
    unlock_table ();
}


/**
 * gpgme_register_idle:
 * @fnc: Callers idle function
 * 
 * Register a function with GPGME called by GPGME whenever it feels
 * that is is idle.  NULL may be used to remove this function.
 **/
void
gpgme_register_idle ( void (*fnc)(void) )
{
    idle_function = fnc;
}


static void
run_idle ()
{
    _gpgme_gpg_housecleaning ();
    if (idle_function)
        idle_function ();
}

