/* ---------------------------------------------------------------
  Programmer: Andrew Lim
  Instructor: Dr. Yang Peng 
  Class: CSS 430, Operating Systems
  Task: This assignment helps you understand the replacement mechanism
  		and its performance improvement by implementing a buffer cache that
  		stores frequently-accessed disk blocks onto the memory. To be more
  		specific, you will implement a second-chance algorithm for your buffer
  		cache, measure its performance when running various test cases, and
  		consider the merits and demerits of your implementation.

  File Created: 11/30/2024
  Last Modified: 12/04/2024
----------------------------------------------------------------*/
import java.util.*;

public class Cache {
    private int blockSize;            // 512 bytes
    private Vector<byte[]> pages;     // This is actual pages that include data
    private int victim;

    private class Entry {
		public static final int INVALID = -1;
		public boolean reference;
		public boolean dirty;
		public int frame;

		public Entry( ) {
	    	reference = false;
	    	dirty = false;
	    	frame = INVALID;
		}
    }

    // This is a page table that includes only attributes
    private Entry[] pageTable = null;

	/* ----------------------------------------------------------
		int findFreePage()
			
		If the frame ID of an entry in the page table is INVALID,
		then this page is considered free
	---------------------------------------------------------- */
    private int findFreePage() {
		//Start Traversing the page table
		for (int i = 0; i < pageTable.length; i++) {
			//If the frame ID of an entry in the page table is INVALID, then this page is considered free
			if (pageTable[i].frame == Entry.INVALID) {
				return i;
			}
		}

		return -1; //Since no free frame found, return -1
    }

	/* ----------------------------------------------------------
		int nextVictim()
			
		Description: If the reference bit of an entry in the page table is FALSE,
		this page is selected as a victim page and returned from this] method;
		otherwise, the reference bit shall be cleared (second chance!).
	---------------------------------------------------------- */	
    private int nextVictim( ) {
		while (true) {
			//Check if the reference bit of an entry in the page table is FALSE
			if (pageTable[victim].reference == false) {
				//If the reference bit is false, select this page as the victim
				victim = (victim + 1) % pageTable.length; // Advance the pointer in circular manner
				return victim;
			} else {
				//If the reference bit is true, give it a second chance by resetting the bit to 0
				pageTable[victim].reference = false;
				victim = (victim + 1) % pageTable.length;
			}
		}
    }

	/* ----------------------------------------------------------
		void writeback (int victimEntry)

		If the “victimEntry” page’s frame is valid and its dirty
		bit is TRUE, this “victimEntry” shall be written back by
		using “SysLib.rawwrite()”. Do remember to clear the dirty
		bit after writing back this victimEntry.
	---------------------------------------------------------- */
    private void writeBack( int victimEntry ) {
		//Check if the “victimEntry” page’s frame is valid and its dirty bit is TRUE
		if (pageTable[victimEntry].frame != Entry.INVALID && pageTable[victimEntry].dirty == true) {
			System.out.println("writeBack: " + victimEntry);	//System.out.println( "writeBack: " + victimEntry );
			SysLib.rawwrite(pageTable[victimEntry].frame, pages.get(victimEntry)); //this “victimEntry” shall be written back by using “SysLib.rawwrite()”
			pageTable[victimEntry].dirty = false;	// Clear the dirty bit after writing back this victimEntry.
		}
    }

	/* ----------------------------------------------------------
		Cache( int blockSize, int cacheBlocks ) 

		The constructor: allocates a cacheBlocks number of cache
		blocks, each containing blockSize-byte data, on memory
	---------------------------------------------------------- */
 	// cacheBlocks = 10, blockSize = 512
    public Cache( int blockSize, int cacheBlocks ) {
		this.blockSize = blockSize;
		pages = new Vector<byte[]>( );
		
		for ( int i = 0; i < cacheBlocks; i++ ) {
	    	byte[] p = new byte[blockSize];
	    	pages.addElement( p );
		}

		victim = cacheBlocks - 1; // set the last frame as a previous victim
		pageTable = new Entry[ cacheBlocks ];
	
		for ( int i = 0; i < cacheBlocks; i++ )
		    pageTable[i] = new Entry( );
    }

	/* ----------------------------------------------------------
		boolean read(int blockId, byte buffer[])

		Reads into the buffer[ ] array the cache block specified by
		blockId from the disk cache if it is in cache, otherwise reads
		the corresponding disk block from the disk device. Upon an
		error, it should return false, otherwise return true.
	---------------------------------------------------------- */
    public synchronized boolean read( int blockId, byte buffer[] ) {
		if ( blockId < 0 ) {
	    	SysLib.cerr( "threadOS: a wrong blockId for cread\n" );
	    	return false;
		}

		//Page Hit Implementation
		//Start Traversing the pageTable to look for an entry whose frame is “blockId”.
		for (int i = 0; i < pageTable.length; i++) {
			//If the target blockId is found, return true; otherwise, it is a page miss
        	if (pageTable[i].frame == blockId) {
	            System.arraycopy(pages.get(i), 0, buffer, 0, blockSize); //use “System.arraycopy()” to READ DATA FROM THE PAGE INTO BUFFER.
    	        pageTable[i].reference = true; //set the reference bit to TRUE
        	    return true; // Page hit
        	}
    	}

		//Page Miss Implementation
    	int victimEntry;

		//Look for a free page at first by calling findFreePage().
		victimEntry = findFreePage(); //findFreePage() will return -1 if it doesnt find a free frame

		//Check if findFreePage() returns -1
		//If you cannot find a free page, then:
		if (victimEntry == -1) {
			victimEntry = nextVictim();		//you should use nextVictim() to identify a victim page.
		}

		//Write back a dirty copy
		writeBack( victimEntry );

		//Read a requested block from disk
		SysLib.rawread( blockId, buffer );

		//Cache a newly read block after calling SysLib.rawread( blockId, buffer )
		System.arraycopy(buffer, 0, pages.get(victimEntry), 0, blockSize);	//Update the actual data of “victimEntry” to the data saved into “buffer”.
		pageTable[victimEntry].frame = blockId;		//Update the frame ID of “victimEntry” 
		pageTable[victimEntry].reference = true;	//Update the reference bit of “victimEntry” to true
		pageTable[victimEntry].dirty = false;		//Update the dirty bit to false because we are reading

		return true;
    }

	/* ----------------------------------------------------------
		boolean write(int blockId, byte buffer[])

		Writes the buffer[ ]array contents to the cache block specified
		by blockId from the disk cache if it is in cache, otherwise finds
		a free cache block and writes the buffer [ ] contents on it. No
		write through. Upon an error, it should return false, otherwise
		return true.
	---------------------------------------------------------- */
    public synchronized boolean write( int blockId, byte buffer[] ) {
		if ( blockId < 0 ) {
	    	SysLib.cerr( "threadOS: a wrong blockId for cwrite\n" );
			return false;
		}

		//Page Hit Implementation
		//Start Traversing the pageTable to look for an entry whose frame is “blockId”.
		for (int i = 0; i < pageTable.length; i++) {
			//If the target blockId is found, return true; otherwise, it is a page miss
        	if (pageTable[i].frame == blockId) {
            	//Locate a valid page and copy the hit page to buffer
	            System.arraycopy(buffer, 0, pages.get(i), 0, blockSize);		//Use “System.arraycopy()” to BUFFER INTO THE PAGE.
    	        pageTable[i].reference = true; //Set the reference bit to TRUE
				pageTable[i].dirty = true; //Set the dirty bit to TRUE
        	    return true; // Page hit
        	}
    	}
    
		//Page Miss Implementation
    	int victimEntry;

		//Look for a free page at first by calling findFreePage().
		victimEntry = findFreePage(); //findFreePage() will return -1 if it doesnt find a free frame

		//Check if findFreePage() returns -1
		//If you cannot find a free page, then:
		if (victimEntry == -1) {
			victimEntry = nextVictim();		//you should use nextVictim() to identify a victim page.
		}

		// write back a dirty copy
		writeBack(victimEntry);

		//Cache it but not write through.
		System.arraycopy(buffer, 0, pages.get(victimEntry), 0, blockSize); //Update the actual data of “victimEntry” to the data in “buffer”
		pageTable[victimEntry].frame = blockId;		//Update the frame ID of the victimEntry
		pageTable[victimEntry].reference = true;	//Update the reference bit to TRUE
		pageTable[victimEntry].dirty = true;		//Update the dirty bit to TRUE because it's being written to

		return true; 
    }

    public synchronized void sync( ) {
		for ( int i = 0; i < pageTable.length; i++ )
	    	writeBack( i );
		
		SysLib.sync( );
    }

    public synchronized void flush( ) {
		for ( int i = 0; i < pageTable.length; i++ ) {
	    	writeBack( i );
	    	pageTable[i].reference = false;
	    	pageTable[i].frame = Entry.INVALID;
		}

		SysLib.sync( );
    }
}
