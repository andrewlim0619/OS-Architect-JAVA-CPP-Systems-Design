/* ---------------------------------------------------------------
  File: Source File, Provided Driver File
  Instructor: Dr. Yang Peng 
  Class: CSS 430, Operating Systems
  Purpose: To run sthread.cpp file

  File Created: 10/22/2024
  Last Modified: 10/22/2024
----------------------------------------------------------------*/
#include "sthread.cpp"

void func1( void *arg ) {
  sthread_init( );
  for ( int i = 0; i < 10; i++ ) {
    cout << "func1: " << (char *)arg << " " <<  i << endl;
    sleep( 1 );
    sthread_yield( );
  }
  sthread_exit( );
}

void func2( void *arg ) {
  sthread_init( );
  for ( int i = 0; i < 10; i++ ) {
    cout << "func2: " << (char *)arg << " " <<  i << endl;
    sleep( 1 );
    sthread_yield( );
  }
  sthread_exit( );
}

void func3( void *arg ) {
  sthread_init( );
  for ( int i = 0; i < 10; i++ ) {
    cout << "func3: " << (char *)arg << " " <<  i << endl;
    sleep( 1 );
    sthread_yield( );
  }
  sthread_exit( );
}

int main( void ) {
  // initialize the thread scheduler
  scheduler_init( );

  // start user threads
  sthread_create( func1, (void *)"Bothell" );
  sthread_create( func2, (void *)"Seattle" );
  sthread_create( func3, (void *)"Tacoma" );

  // start the sceduler
  scheduler_start( );

  return 0;
}


