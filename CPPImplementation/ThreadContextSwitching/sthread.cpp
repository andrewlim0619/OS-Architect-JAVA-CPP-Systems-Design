/* ---------------------------------------------------------------
  Programmer: Andrew Lim
  Instructor: Dr. Yang Peng 
  Class: CSS 430, Operating Systems
  Task: This assignment implements a very simple “user thread library” and its round-robin scheduler, say sthread (not
        pthread). It exercises how to capture and restore the current execution environment with setjmp( ) and longjmp( ), to
        retrieve stack and base pointers from the CPU register set with asm( ), and to copy the current activation record (i.e.,
        the stack area of the current function) into a scheduler’s memory space upon a context switch to a next thread. We
        also use signal( ) and alarm( ) to guarantee a minimum time quantum to run the current thread.

  File Created: 10/22/2024
  Last Modified: 10/27/2024
  
  NOTE: TODO Functions: capture() and sthread_yield
----------------------------------------------------------------*/
#include <setjmp.h> // setjmp( )
#include <signal.h> // signal( )
#include <unistd.h> // sleep( ), alarm( )
#include <stdio.h>  // perror( )
#include <stdlib.h> // exit( )
#include <iostream> // cout
#include <string.h> // memcpy
#include <queue>    // queue

#define scheduler_init( ) {			    \
  if ( setjmp( main_env ) == 0 )		\
    scheduler( );				            \
}

#define scheduler_start( ) {			  \
  if ( setjmp( main_env ) == 0 )		\
    longjmp( scheduler_env, 1 );		\
}

/* capture() ---------
  Captures the current thread’s jmp_env and activation
  record into cur_tcb. This is a helper function called from
  sthread_init( ) and sthread_yield( ).
*/
#define capture() {							                                                                        \
  /*                                                                                                    \
    asm( ) statements for defining registers to match “sp” and “bp” in system                           \
    sp --> stack pointer (Top)                                                                          \
    bp --> base pointer (bottom)                                                                        \
  */                                                                                                    \
  register void *sp asm("sp");  /*Stores the stack poiner into sp*/                                     \
  register void *bp asm("bp");  /*Stores the base poiner into sp*/                                      \
                                                                                                        \
  /*Saving into TCB the size of the stacks and it's pointer*/                                           \
  cur_tcb->size = (int)((long long int)bp - (long long int)sp);                                         \
  cur_tcb->sp = sp;                                                                                     \
  cur_tcb->stack = malloc(cur_tcb->size); /*Allocate Enough memory for the stack*/                      \
  memcpy( cur_tcb->stack, sp, cur_tcb->size );                                                          \
                                                                                                        \
  thr_queue.push(cur_tcb);  /*Push current TCB into a queue of active threads*/                         \
}

/* sthread_yield() ---------
  Is called by each user thread to voluntarily yield the
  CPU. Only after a timer interrupt, calls capture( ) and
  goes back to the scheduler. When the control comes
  back from the scheduler, retrieve this thread activation
  record from cur_tcb->stack.
*/
#define sthread_yield() {                                                                               \
  if (alarmed ==  true) {                                                                               \
    alarmed = false;  /*Reset alarm flag*/                                                              \
    if (setjmp(cur_tcb->env) == 0) {                                                                    \
      capture();                                                                                        \
      longjmp(scheduler_env, 1);  /*Go back to last saved execution state*/                             \
    }                                                                                                   \
    memcpy(cur_tcb->sp, cur_tcb->stack, cur_tcb->size); /*Retrieve Stack*/                              \
  }                                                                                                     \
}

#define sthread_init( ) {					                      \
  if ( setjmp( cur_tcb->env ) == 0 ) {			            \
    capture( );						                              \
    longjmp( main_env, 1 );					                    \
  }								                                      \
  memcpy( cur_tcb->sp, cur_tcb->stack, cur_tcb->size );	\
}

#define sthread_create( function, arguments ) {         \
  if ( setjmp( main_env ) == 0 ) {		                  \
    func = &function;				                            \
    args = arguments;				                            \
    thread_created = true;			                        \
    cur_tcb = new TCB( );			                          \
    longjmp( scheduler_env, 1 );		                    \
  }						                                          \
}

#define sthread_exit( ) {			    \
  if ( cur_tcb->stack != NULL )		\
    free( cur_tcb->stack );			  \
  longjmp( scheduler_env, 1 );		\
}

using namespace std;

static jmp_buf main_env;
static jmp_buf scheduler_env;

// Thread control block
class TCB {
public:
  TCB( ) : sp( NULL ), stack( NULL ), size( 0 ) { }
  jmp_buf env;  // the execution environment captured by set_jmp( )
  void* sp;     // the stack pointer 
  void* stack;  // the temporary space to maintain the latest stack contents
  int size;     // the size of the stack contents
};
static TCB* cur_tcb;   // the TCB of the current thread in execution

// The queue of active threads
static queue<TCB*> thr_queue;

// Alarm caught to switch to the next thread
static bool alarmed = false;
static void sig_alarm( int signo ) {
  alarmed = true;
}

// A function to be executed by a thread
void (*func)( void * );
void *args = NULL;
static bool thread_created = false;

static void scheduler( ) {
  // invoke scheduler
  if ( setjmp( scheduler_env ) == 0 ) {
    cerr << "scheduler: initialized" << endl;
    if ( signal( SIGALRM, sig_alarm ) == SIG_ERR ) {
      perror( "signal function" );
      exit( -1 );
    }
    longjmp( main_env, 1 );
  }

  // check if it was called from sthread_create( )
  if ( thread_created == true ) {
    thread_created = false;
    ( *func )( args );
  }

  // restore the next thread's environment
  if ( ( cur_tcb = thr_queue.front( ) ) != NULL ) {
    thr_queue.pop( );

    // allocate a time quontum of 5 seconds
    alarm( 5 );

    // return to the next thread's execution
    longjmp( cur_tcb->env, 1 );
  }

  // no threads to schedule, simply return
  cerr << "scheduler: no more threads to schedule" << endl;
  longjmp( main_env, 2 );
}
