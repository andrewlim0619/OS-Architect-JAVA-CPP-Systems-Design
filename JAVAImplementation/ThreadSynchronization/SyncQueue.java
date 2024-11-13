package ThreadSynchronization;
/*
 * This is a template of SyncQueue.java. Chagne this file name into SyncQueue.java and
 * complete the implementation
 */

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

    public SyncQueue( ) {
    // TODO: implement this constructor
	// Assume SyncQueue( COND_MAX );
    }

    public SyncQueue( int condMax ) {
    // TODO: implement this constructor
    }

    int enqueueAndSleep( int condition ) {
    // TODO: implement this method
	// Verify the correctness of condition.
	// Call the corresponding queue[ ].sleep( ).
	// Return the corresponding child thread ID.
    }

    void dequeueAndWakeup( int condition, int tid ) {
    // TODO: implement this method
	// verify the correctness of condition.
	// Call the corresponding queue[ ].wakeup( ... );
    }

    void dequeueAndWakeup( int condition ) {
    // TODO: implement this method
	// Assume tid = 0.
    }
}
