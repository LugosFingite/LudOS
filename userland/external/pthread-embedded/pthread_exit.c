/*
 * pthread_exit.c
 *
 * Description:
 * This translation unit implements routines associated with exiting from
 * a thread.
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-embedded (PTE) - POSIX Threads Library for embedded systems
 *      Copyright(C) 2008 Jason Schmidlapp
 *
 *      Contact Email: jschmidlapp@users.sourceforge.net
 *
 *
 *      Based upon Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 *
 *      Contact Email: rpj@callisto.canberra.edu.au
 *
 *      The original list of contributors to the Pthreads-win32 project
 *      is contained in the file CONTRIBUTORS.ptw32 included with the
 *      source code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "pthread.h"
#include "implement.h"

void
pthread_exit (void *value_ptr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function terminates the calling thread, returning
 *      the value 'value_ptr' to any joining thread.
 *
 * PARAMETERS
 *      value_ptr
 *              a generic data value (i.e. not the address of a value)
 *
 *
 * DESCRIPTION
 *      This function terminates the calling thread, returning
 *      the value 'value_ptr' to any joining thread.
 *      NOTE: thread should be joinable.
 *
 * RESULTS
 *              N/A
 *
 * ------------------------------------------------------
 */
{
  pte_thread_t * sp;

  /*
   * Don't use pthread_self() to avoid creating an implicit POSIX thread handle
   * unnecessarily.
   */
  sp = (pte_thread_t *) pthread_getspecific (pte_selfThreadKey);

  if (NULL == sp)
    {
      /*
       * A POSIX thread handle was never created. I.e. this is a
       * Win32 thread that has never called a pthreads-win32 routine that
       * required a POSIX handle.
       *
       * Implicit POSIX handles are cleaned up in pte_throw() now.
       */

      /* Terminate thread */
      pte_osThreadExit();

      /* Never reached */
      __builtin_unreachable();
    }

  sp->exitStatus = value_ptr;

  pte_throw (PTE_EPS_EXIT);

  /* Never reached. */
  __builtin_unreachable();
}