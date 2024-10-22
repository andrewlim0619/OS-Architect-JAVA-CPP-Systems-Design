/* ---------------------------------------------------------------
  Programmer: Andrew Lim
  Instructor: Dr. Yang Peng 
  Class: CSS 430, Operating Systems
  Task: This assignment is designed to help you understand that,
        from the kernel’s viewpoint, the shell is simply an application
        program that uses system calls to spawn and to terminate other
        user programs. You will also get familiar with our ThreadOS
        operating system simulator through the assignment.
  File Created: 10/15/2024
  Last Modified: 10/20/2024
  
  NOTE: File renamed to Shell.java
----------------------------------------------------------------*/
import java.io.*;
import java.util.*;

/**
 * Is a ThreadOS Shell interpreter.
 */
public class Shell extends Thread
{
    /**
     * Is a default constructor.
     */
    public Shell( ) {
    }

    /**
     * Is the Shell.java thread's main body. It repeatedly prints a
     * command prompt "shell[number]% ", reads a line of user
     * commands, interprets each command to launch the corresponding
     * user thread, and check with a command delimitor "&amp;" or ";"
     * whether it should wait for the termination or go onto the next
     * command interpretation.
     */
    public void run( ) {
        /**
         *  Variable Declaration and Initialization
         */
        Set<Integer> activePIDs = new HashSet<>();   // Use a Set to store the process IDs to keep track and manage concurrent processes

        for ( int line = 1; ; line++ ) {
            /**
            * Do-While Loop
            * Purpose: After prompting a "Shell[x] command prompt, the porgram will take in a user input and convert the 
            * command line using an input buffer into a String. cmdLine = Whatever Command Line is being written.
            */
            String cmdLine = "";
            do { // print out a command prompt and read one line.
                StringBuffer inputBuf = new StringBuffer( );
                SysLib.cerr( "Shell[" + line + "]% " );
                SysLib.cin(inputBuf);
                cmdLine = inputBuf.toString();
            } while (cmdLine.length( ) == 0); //CHECK!

            /**
             * String[] args - Contains all of the "terms" in String cmdLine and stores each terms in an index
             */
            String[] args = SysLib.stringToArgs(cmdLine);
            int first = 0;

            /**
             * For loop is used to iterate through all terms inside String array "args"
             */
            for (int i = 0; i < args.length; i++) {
                // If at index i inside args is a delimiter (& or ;) or if the i is the last term, enter. Else, increment i and move on to next term
                if (args[i].equals( ";" ) || args[i].equals( "&" ) || i == args.length - 1) {
                    String[] command = generateCmd(args, first, ( i==args.length - 1 ) ? i+1 : i); //Command[] contains a total of 3 values, including delimeter
                    
                    /**
                     * TODO Shell_hw1B Task
                     */
                    if (command != null) {
                        if (command[0].equals("exit")) {
                            SysLib.exit();
                            return;
                        }
                        
                        int PID = SysLib.exec(command); //A new process has been created and it's process id is stored

                        if (PID != -1) { //If PID is not -1, This means that the exec command was successful and the creation of a process is successful
                            activePIDs.add(PID); //Since exec passes, that means we have created a new process and we should add it to the set.

                            /**
                             * If condition to check what the delimiter is. Hence, if the delimieter at the end of each respective command is:
                             * a. ';' --> This means Sequential Execution
                             * b. '&' --> This means Concurrent Execution (Run other processes in the background)
                             */
                            //Check for ; first as to ensure that we execute and finish waiting for all active processes (Currently running and running in the background)
                            if (args[i].equals(";") || i == args.length - 1) {
                                for (Integer pid : activePIDs) {
                                    SysLib.join();  // Join all exisiting PID inside of the active PIDs
                                }

                                activePIDs.clear(); //Since we have join and finished executing all active processes, we can clear the set
                            }
                        }
                        //Enter this else block if SysLib.exec() failes
                        else {
                            SysLib.exit();
                            return;
                        }  
                    }

                    first = i + 1;
                }
            }
        }
    }

    /**
     * returns a String array of a command and its arguments excluding
     * the delimiter ';' or '&amp;'.
     */
    private String[] generateCmd( String args[], int first, int last ) {
        if ( (args[last-1].equals(";")) || (args[last-1].equals("&")) )
            last = last -1;

        if ( last - first <= 0 )
            return null;
        String[] command = new String[ last - first ];
        for ( int i = first ; i < last; i++ ) 
              command[i - first] = args[i];
        return command;
    }
}