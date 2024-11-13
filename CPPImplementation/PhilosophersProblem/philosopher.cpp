/* ---------------------------------------------------------------
  Programmer: Andrew Lim
  Instructor: Dr. Yang Peng 
  Class: CSS 430, Operating Systems
  Task: This assignment solves the dining philosophers problem,
        using the pthread library. You will exercise how to use:
        pthread_mutex_init/lock/unlock and pthread_cond_init/wait/signal
        to implement a monitor. The best solution is to use a monitor that
        guarantees each philosopher to pick up both left and right chopsticks
        simultaneously. An alternative but quite inefficient solution is to
        let a philosopher hog the entire table so that any other philosopher
        must wait untils/he is done with eating. While it’s easy, it does
        not allow any concurrency among the philosophers.

  File Created: 11/05/2024
  Last Modified: 11/10/2024
----------------------------------------------------------------*/
#include <pthread.h> // pthread
#include <stdio.h>
#include <stdlib.h>   // atoi( )
#include <unistd.h>   // sleep( )
#include <sys/time.h> // gettimeofday
#include <iostream>   // cout

#define PHILOSOPHERS 5
#define MEALS 3

using namespace std;

class Table2 {
  public:
    Table2( ) {
      //Initializing the table
      for (int i = 0; i < PHILOSOPHERS; i++) {
        pthread_mutex_init(&lock, NULL);      //Initialize the mutex
        pthread_cond_init(&self[i], NULL);    //Initialize the conditional variable
        state[i] = THINKING;      //Initialize the state
      }
    }

    void pickup( int i ) { 
      pthread_mutex_lock(&lock);    //Lock the mutex immediately
      state[i] = HUNGRY;    //Philosopher is hungry, meaning they want to eat
      test(i);    //Check if the philosopher can start eating (based on neighbors' states)

      //If the philosopher was unable to start eating (state[i] is not EATING),
      //they must wait until they are signaled that they can eat.
      if (state[i] != EATING) {
        pthread_cond_wait(&self[i], &lock);
      }

      cout << "philosopher[" << i << "] picked up chopsticks" << endl;
      pthread_mutex_unlock(&lock);    //Unlock the mutex
    }

    void putdown( int i ) { 
      pthread_mutex_lock(&lock);    //Lock the mutex immediately
      state[i] = THINKING;    //Philosopher is thinking, meaning they finished eating

      //Logic: i(L,R)
      //1(0,2) | 2(1,3) | 3(2,4) | 4(3,0) --> I is next to left and right neighbours
      test((i + 4) % PHILOSOPHERS);   //Check the left neighbor
      test((i + 1) % PHILOSOPHERS);   //Check the right neighbor

      cout << "philosopher[" << i << "] put down chopsticks" << endl;
      pthread_mutex_unlock(&lock); // Unlock the mutex
    }

  private:
    //private data members
    enum { THINKING, HUNGRY, EATING } state[PHILOSOPHERS];
    pthread_mutex_t lock;
    pthread_cond_t self[PHILOSOPHERS];
  
    //private method, called in pickup() and putdown()
    void test( int i ) {
      //Only enter the if condition when:
      //Current philosopher must be hungry
      //Left && Right neighbour cannot be eating if current philosopher is eating
      if ((state[i] == HUNGRY) && (state[(i + 4) % PHILOSOPHERS] != EATING) && (state[(i + 1) % PHILOSOPHERS] != EATING)) {
        state[i] = EATING;
        pthread_cond_signal(&self[i]);
      }
    }
};

class Table1 {
  public:
    Table1( ) {
      pthread_mutex_init( &lock, NULL );    //Initializing the mutex lock
    }

    void pickup( int i ) {
      pthread_mutex_lock(&lock);    //Locking the mutex 
      cout << "philosopher[" << i << "] picked up chopsticks" << endl;
    }

    void putdown( int i ) {
      cout << "philosopher[" << i << "] put down chopsticks" << endl;
      pthread_mutex_unlock(&lock);    //Unlocking the mutex
    }

  private:
    pthread_mutex_t lock;   //Defining a mutex lock
};

class Table0 {
  public:
    void pickup( int i ) {
      cout << "philosopher[" << i << "] picked up chopsticks" << endl;
    }

    void putdown( int i ) {
      cout << "philosopher[" << i << "] put down chopsticks" << endl;
    }
};

static Table2 table2;
static Table1 table1;
static Table0 table0;

static int table_id = 0;

void *philosopher( void *arg ) {
  int id = *(int *)arg;
  
  for ( int i = 0; i < MEALS; i++ ) {
    switch( table_id ) {
      case 0:
        table0.pickup( id );
        sleep( 1 );
        table0.putdown( id );
        break;
      case 1:
        table1.pickup( id );
        sleep( 1 );
        table1.putdown( id );
        break;
      case 2:
        table2.pickup( id );
        sleep( 1 );
        table2.putdown( id );
        break;
    }
  }
  return NULL;
}

int main( int argc, char** argv ) {
  pthread_t threads[PHILOSOPHERS];
  pthread_attr_t attr;
  int id[PHILOSOPHERS];
  table_id = atoi( argv[1] );

  pthread_attr_init(&attr);
  
  struct timeval start_time, end_time;
  gettimeofday( &start_time, NULL );
  for ( int i = 0; i < PHILOSOPHERS; i++ ) {
    id[i] = i;
    pthread_create( &threads[i], &attr, philosopher, (void *)&id[i] );
  }

  for ( int i = 0; i < PHILOSOPHERS; i++ )
    pthread_join( threads[i], NULL );
  gettimeofday( &end_time, NULL );

  sleep( 1 );
  cout << "time = " << ( end_time.tv_sec - start_time.tv_sec ) * 1000000 + ( end_time.tv_usec - start_time.tv_usec ) << endl;

  return 0;
}
