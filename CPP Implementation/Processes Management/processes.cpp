/* ---------------------------------------------------------------
  Programmer: Andrew Lim
  Instructor: Dr. Yang Peng 
  Class: CSS 430, Operating Systems
  Task: This assignment is designed to introduce how to manage processes;
        creating a child process, executing a new program in the child process,
        communicating with it, remapping this communication channel to a different
        filedescriptor, and waiting for the child to finish
  File Created: 10/06/2024
  Last Modified: 10/13/2024
  
  NOTE: <sys/wait.h> & <unistd.h> are UNIX Specific Header Files
----------------------------------------------------------------*/

#include <sys/types.h>   // for fork, wait
#include <sys/wait.h>    // for wait
#include <unistd.h>      // for fork, pipe, dup, close
#include <stdio.h>       // for NULL, perror
#include <stdlib.h>      // for exit
#include <iostream>      // for cout

using namespace std;

int main (int argc, char** argv) {
  //Variables Initialization and Declaration
  int fds[2][2];
  int pid;

  /*
    Variables for reading and writing Pipe1 and Pipe2
    Child ()--Pipe1--) GrandChild || GrandChild ()--Pipe2--) Great Grand Child
      -> Child Communicates with Grand Child using Pipe1
      -> Grand Child Communicates with Great Grand Child using Pipe2
  */
  int readPipe1, writePipe1, readPipe2, writePipe2 = 0;
  int pipes[] = {readPipe1, writePipe1, readPipe2, writePipe2};

  //Function Call
  int checkExeclp(int, string);

  //Validate User Input
  if (argc != 2) {
    cerr << "Usage: processes command" << endl;
    exit(-1);
  }

  //Validate if pipes are created successfully. Returns 0 if successfully created
  if (pipe(fds[0]) != 0 || pipe(fds[1]) != 0) {
    perror("pipe creation Error");      
  }

  /*
    Start of Fork()
  */
  pid = fork();   //PARENT fork() -> CHILD
  readPipe1 = fds[0][0];    //Read Pipe1
  writePipe1 = fds[0][1];   //Write Pipe1
  readPipe2 = fds[1][0];    //Read Pipe2
  writePipe2 = fds[1][1];   //Wrtie Pipe2

  if (pid < 0) {
    perror("fork error");   //Validate fork() right after PARENT creates CHILD process
    exit(-1);
  } 
  else if (pid == 0) {
    pid = fork();   //CHILD fork() -> GRAND CHILD

    if (pid < 0) {
      perror("fork error");   //Validate fork() right after CHILD creates GRAND CHILD process
      exit(-1);
    } 
    else if (pid == 0) {
      pid = fork();  //GRAND CHILD fork() -> GREAT GRAND CHILD

      if (pid < 0) {
        perror("fork error");   //Validate fork() right after GRAND CHILD creates GREAT GRAND CHILD process
        exit(-1);
      } 
      else if (pid == 0) {
        //I'm a Great Grand Child, hence execute "ps" 
        //cout << "In Great Grand Child Process: Executing 'ps -A'" << endl;    //Debugging Purposes
        close(readPipe1);     //Pipe 1 Read End will not be used by Great Grand Child to communicate with Grand Child, hence close it
        close(writePipe1);    //Pipe 1 Write End will not be used by Great Grand Child to communicate with Grand Child, hence close it   
        close(readPipe2);     //Pipe 2 Read End not used because we want to write output of Great Grand Child to Grand Child through Pipe 2, hence close it     
        dup2(writePipe2, 1);  //Redirect stdout to Pipe 2 Write End to send output of execlp() to Grand Child

        checkExeclp(execlp("ps", "ps", "-A", NULL), "Great Grand Child");
      } 
      else {
        //I'm a Grand Child, hence execute "grep" 
        //cout << "In Grand Child Process: Executing 'grep argv[1]'" << endl;   //Debugging Purposes
        close(readPipe1);     //Pipe 1 Read End not used because we want to write output of Grand Child to Child through Pipe 1, hence close it       
        dup2(writePipe1, 1);  //Redirect stdout to Pipe 1 Write End to write output of execlp() to Child
        dup2(readPipe2, 0);   //Redirect stdin to Pipe 2 Read End so that Grand Child can read output of execlp() from Great Grand Child     
        close(writePipe2);    //Pipe 2 Write End not used because Child only needs to read what is inside Pipe 2 written by Great Grand Child       
        wait(NULL);           //Wait to execute until Great Grand Child Process ends

        checkExeclp(execlp("grep", "grep", argv[1], NULL), "Grand Child");  
      }
    } 
    else {
      //I'm a child, hence execute "wc"
      dup2(readPipe1, 0);     //Redirect stdin to Pipe 1 Read End so that Child can read output of execlp() from  Grand Child
      close(writePipe1);      //Pipe 1 Write End not used because Child only needs to read what is inside Pipe 1 written by Grand Child   
      close(readPipe2);       //Pipe 2 Read End will not be used by Child to communicate with Grand Child, hence close it
      close(writePipe2);      //Pipe 2 Write End will not be used by Child to communicate with Grand Child, hence close it  
      wait(NULL);             //Wait to execute until Grand Child Process ends

      //cout << "In Child Process: Executing 'wc -l'" << endl;    //Debugging Purposes 
      checkExeclp(execlp("wc", "wc", "-l", NULL), "Child");
    }
  } 
  else {
    //I'm a parent, hence ensure to close all pipes
    for (int i = 0; i < sizeof(pipes); i++) {
      close(pipes[i]);
    }
    wait(NULL);   //Wait to execute until Child Process ends
    
    //cout << "In Parent Process: Fork() Execution Completed" << endl;  //Debugging Purposes 
    cout << "commands completed" << endl;
  }
}

//Function to check if execlp is successful. If unsuccessful, print out error and terminate process
int checkExeclp(int execStatus, string process) {
  if (execStatus == -1) {
    string errorMsg = "unsuccessful execlp() in " + process + " process";
    perror(errorMsg.c_str());
    exit(-1);   //Terminate
  }

  return execStatus;
}