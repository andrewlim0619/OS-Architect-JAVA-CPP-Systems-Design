/* ---------------------------------------------------------------
  Programmer: Andrew Lim
  Instructor: Dr. Yang Peng 
  Class: CSS 430, Operating Systems
  Task: This assignment implements your own malloc( ) and free( )
        using the “sbrk” system function: change data segment size.
        Your implementation is based on the first-fit and the best-fit
        strategies. Since the original malloc/free functions in Linux
        use “brk” (an even more legacy function than sbrk), you can compare
        your own and the Linux-original implementations in terms of # brk 
        system function calls. (The fewer calls, the better memory allocations.)

  File Created: 11/20/2024
  Last Modified: 11/27/2024
----------------------------------------------------------------*/
#include <unistd.h> // sbrk( )
#include <limits.h> // LONG_MAX

using namespace std;

static bool initialized = false;
static void *heap_top; // the beginning of the heap space
static void *heap_end; // the current boundary of the heap space, obtained from sbrk( 0 )

class MCB { // memory control block
    public:
    int available; // true(1): this memory partition is available, false(0) unavailalbe.
    int size;      // MCB size + the user data size
};

void free_( void *dealloc_space ) {
    MCB *mcb; 

    // locate this partition's mcb address from dealloc_space, deallocated space follows its mcb;
    mcb = (MCB *)((char *)dealloc_space - sizeof(MCB));
    mcb->available = true;
    return;
}

/*
    First fit: allocate the first hole that is big enough. Searching can start from the
    beginning of the heap. We will stop searching as soon as we find a free hole that is
    large enough
*/
void *malloc_f(long size) {
    struct MCB *cur_mcb;          // current MCB
    void *new_space = NULL; // this is a pointer to a new memory space allocated for a user
    
    if (!initialized) {
        // find the end of heap memory, upon an initialization
        heap_end = sbrk( 0 );
        heap_top = heap_end;
        initialized = true;
    }

    // append an MCB in front of a requested memroy space
    size += sizeof(MCB);

    // start scanning
    for (void *cur = heap_top; cur < heap_end; cur = (void *)((char *)cur + ((MCB *)cur)->size)) {
        MCB *cur_mcb = (MCB *)cur; // Let cur_mcb point to the current memory block

        // If cur_mcb->available and cur_mcb->size fits size
        if (cur_mcb->available && cur_mcb->size >= size) {
            cur_mcb->available = false; // Since cur_mcb is used, it is no longer available
            new_space = (void *)((char *)cur_mcb + sizeof(MCB)); // new_space points to this MCB

            return new_space; // Return the pointer to the allocated memory
        }
    }

    // no space found yet
    if (new_space == NULL) {
        new_space = sbrk(size); // get a space from OS and old boundary now becomes new_space,

        // initialize cur_mcb with new_space and size.
        cur_mcb = (MCB *)new_space;
        cur_mcb->size = size;
        cur_mcb->available = false;     //No longer available

        heap_end = (void *)((char *)heap_end + size);
    }

    // new space is after new MCB
    return (void *)( ( long long int )new_space + sizeof( MCB ) );
}

/*
    Best fit: Allocate the smallest hole that is big enough. We must search through the
    heap entirely. This strategy produces the smallest leftover hole.
*/
void *malloc_b(long size) {
    struct MCB *cur_mcb;          // current MCB
    void *new_space = NULL; // this is a pointer to a new memory space allocated for a user

    // Initialize heap boundaries if not already done
    if( !initialized )   {
        // find the end of heap memory, upon an initialization
        heap_end = sbrk( 0 );
        heap_top = heap_end;
        initialized = true;
    }

    // Adjust size to include MCB metadata
    size += sizeof(MCB);
    struct MCB *best_mcb = NULL;

    // start scanning
    for (void *cur = heap_top; cur < heap_end; cur = (void *)((char *)cur + ((MCB *)cur)->size)) {
        MCB *cur_mcb = (MCB *)cur; // Let cur_mcb point to the current memory block

        // If cur_mcb->available and cur_mcb->size fits size
        if (cur_mcb->available && cur_mcb->size >= size) {
            // If cur_mcb->size is the best size so far
            if (best_mcb != NULL || cur_mcb->size < best_mcb->size) {
                best_mcb = cur_mcb; // Temporarily memorize this best size so far and this best mcb so far
            }
        }
    }

    // After scan, check the best mcb so far. If it is not null
    if (best_mcb != NULL) {
        best_mcb->available = false; 
        new_space = (void *)((char *)best_mcb + sizeof(MCB)); // new_space points to this best mcb

        return new_space;
    }

    // no space found yet
    if (new_space == NULL) {
        new_space = sbrk(size); // get a space from OS and old boundary now becomes new_space,

        // initialize cur_mcb with new_space and size.
        cur_mcb = (MCB *)new_space;
        cur_mcb->size = size;
        cur_mcb->available = false;     // no longer available

        heap_end = (void *)((char *)heap_end + size);
    }

    // New space is after new MCB
    return (void *)( ( long long int )new_space + sizeof( MCB ) );
}
