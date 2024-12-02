/* ---------------------------------------------------------------
  Programmer: Andrew Lim
  Instructor: Dr. Yang Peng 
  Class: CSS 430, Operating Systems
  Task: Driver file for malloc.cpp

  File Created: 11/20/2024
  Last Modified: 11/27/2024
----------------------------------------------------------------*/

#include <stdio.h>   // printf( )
#include <stdlib.h>  // rand( )
#include <iostream>  // cout

#define MAX_MEMSIZE 1024
#define N_CHUNKS 10

extern void *malloc_f( long );
extern void *malloc_b( long );
extern void free_( void* );

using namespace std;

char print = 'n';
char libc = 'l';

void *rand_alloc( ) {
  int memsize = rand( ) % MAX_MEMSIZE;
  memsize = ( memsize == 0 ) ? 1 : memsize;
  if ( print == 'p' ) printf( "rand_alloc: size = %d ", memsize );
  return ( libc == 'l' ) ? malloc( memsize ) : 
    ( ( libc == 'f' ) ? malloc_f( memsize ) : malloc_b( memsize ) );
}

int main( int argc, char **argv ) {
  if ( argc >= 2 )
    libc = argv[1][0];
  if ( argc >= 3 )
    print = argv[2][0];

  void *chunks[N_CHUNKS];

  // allocate all chuncks
  for ( int i = 0; i < N_CHUNKS; i++ ) {
    chunks[i] = rand_alloc( );
    if ( print == 'p' ) printf( "chuncks[%d] = %p allocated\n", i, chunks[i] );
  }

  // random de/re-allcation
  for ( int i = 0; i < N_CHUNKS * 10; i++ ) {
    int target = rand( ) % N_CHUNKS;
    int alloc_free = rand( ) % 2;
    if ( alloc_free == 1 && chunks[target] == NULL ) {
      // re-allocation
      chunks[target] = rand_alloc( );
      if ( print == 'p' ) printf( "chuncks[%d] = %p allocated\n", target, chunks[target] );
    }
    else if ( alloc_free == 0 && chunks[target] != NULL )  {
      // de-allocation
      ( libc == 'l' ) ? free( chunks[target] ) : free_( chunks[target] );
      if ( print == 'p' ) printf( "chuncks[%d] = %p freed\n", target, chunks[target] );
      chunks[target] = NULL;
    }
  }

  return 0;
}
