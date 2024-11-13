package ThreadSynchronization;
/*
 * This is a template of QueueNode.java. Change this file name into QueueNode.java
 * and implement the logic.
 */

import java.util.*;

public class QueueNode {
    private Vector<Integer> tidQueue; // maintains a list of child TIDs who called wakeup( ).

    public QueueNode( ) {
    // TODO: implement this constructor
    }

    public synchronized int sleep( ) {
    // TODO: implement this method
	// If tidQueue has nothing, call wait( ).
	// Otherwise, get one child TID from tidQueue.
	// return it.
    }

    public synchronized void wakeup( int tid ) {
    // TODO: implement this method
	// Add this child TID to tidQueue.
	// Notify the parent.
    }
}
