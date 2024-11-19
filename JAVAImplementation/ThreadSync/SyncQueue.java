package ThreadSynchronization;
/* ---------------------------------------------------------------
  Programmer: Andrew Lim
  Instructor: Dr. Yang Peng 
  Class: CSS 430, Operating Systems
  Task: This assignment exercises the implementation of Java monitors.
        While the original Java monitors have only an ability of randomly
        waking up one or all sleeping threads, the implementation of Java
        monitor in this assignment allows threads to sleep on and wake up
        from a different condition specified as an integer. Using this special
        monitor in SynchQueue.java, you will implement SysLib.wait( ) and SysLib.exit( )
        system calls, with which a parent thread can wait for one of its child threads
        to be terminated.

  File Created: 11/12/2024
  Last Modified: 11/17/2024

    * This is a template of SyncQueue.java. Chagne this file name into SyncQueue.java and
    * complete the implementation
--------------------------------------------------------------- */

public class SyncQueue {
    
	// don't add any new data members
	private QueueNode queue[] = null;
    private final int COND_MAX = 10;
    private final int NO_TID = -1;

	// don't change this private function
    private void initQueue( int condMax ) {
		queue = new QueueNode[ condMax ];
		for ( int i = 0; i < condMax; i++ )
			queue[i] = new QueueNode( );
	}

    //Default Constructor
    public SyncQueue() {
        initQueue(COND_MAX);    //Initialize with default value (COND_MAX) if nothing is passed in
    }

    //Constrcutor with a parameter
    public SyncQueue(int condMax) {
        initQueue(condMax);     //Initialize with passed in value (condMax) the user passes in
    }

    /*
        Allows a calling thread to sleep until a given condition is
        satisfied. It returns the ID of a (child) thread that has
        woken the calling thread.
     */
    int enqueueAndSleep( int condition ) {
        //Verify the correctness of condition.
        if (condition < 0 || condition >= queue.length) {
            System.err.println("Error: Invalid condition index");
            return -1; 
        }
        
        int tid = queue[condition].sleep();     //Call the corresponding queue[ ].sleep( ).
        return tid;     //Return the corresponding child thread ID.
    }

    /*
        Dequeues and wakes up a thread waiting for a
        given condition and informing the woken thread of this
        calling thread ID (tid). If there are two or more threads
        waiting for the same condition, only one thread is
        dequeued and resumed. The FCFS (first-come-first-serve)
        order does not matter.
    */
    void dequeueAndWakeup( int condition, int tid ) {
        //Verify the correctness of condition.
        if (condition < 0 || condition >= queue.length) {
            System.err.println("Error: Invalid condition index");
            return; 
        }

        queue[condition].wakeup(tid);       //Call the corresponding queue[ ].wakeup( ... );
    }

    void dequeueAndWakeup( int condition ) {
        //Assume tid = 0
        dequeueAndWakeup(condition, 0);
    }
}
