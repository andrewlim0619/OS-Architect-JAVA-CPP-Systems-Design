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
    
    * This is a template of QueueNode.java. Change this file name into QueueNode.java
    * and implement the logic.
--------------------------------------------------------------- */

import java.util.*;

public class QueueNode {
    private Vector<Integer> tidQueue; // maintains a list of child TIDs who called wakeup( ).

    //Default Constructor
    public QueueNode( ) {
        tidQueue = new Vector<>();      //Create a new vector to maintain a list of child TIDs
    }

    public synchronized int sleep( ) {
        int tid;

        //If tidQueue has nothing, call wait( ).
        if (tidQueue.isEmpty()) {
            wait();
        } else {
            tid = tidQueue.remove(0);       //Otherwise, get one child TID from tidQueue.
        }

        return tid;     //Return the child
    }

    public synchronized void wakeup( int tid ) {
        tidQueue.add(tid);      //Add this child TID to tidQueue
        notify();       //Notify the parent.     
    }
}
