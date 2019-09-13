// Christian Jarani
// Prof. Misurda
// CS 449, Project 5: Shell
// April 24th, 2016

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>

#define BUFSIZE 256

int main()
{
	char input[BUFSIZE];
	char* arguments[30];													 // Holds each strtok'd command
	char* redirect_command;													// Holds command for i/o redirection														
	char* redirect_filename;											   // Holds filename for i/o redirection
	char delimiters[] = {'(',')','|','&',';','\n','\t',' '};      		  // Set delimiter characters for strtok
	int num_of_args = 0;												 // Number of arguments inputted
	int redirect_flag = 0;												// Whether or not i/o is redirected (0 if not, 1 if so)
	int redirect_type;												   // Details what kind of redirection is being done (0 if output, 1 if input)
	FILE* file;														  // File pointer for input redirection
	
	while(1)
	{
		printf("> ");
		fgets(input,BUFSIZE,stdin);								 				   		    // Read user input into input[BUFSIZE] buffer

		if( strchr(input,'<') != NULL || strchr(input,'>') != NULL )		    		  // If input contains an i/o redirection operator
		{
			char redirect_delimiters[] = {'<','>'};					  					// Create delimiter array specific for i/o redirection
			char undelimited[BUFSIZE];
			strcpy(undelimited,input);

			redirect_command = strtok(input,redirect_delimiters);			  	    // Tokenize left side of i/o redirector (contains arguments to be perform)
			redirect_filename = strtok(NULL,delimiters);			 	   		   // Tokenize right side of i/o redirector (contains filename)

			if(strchr(undelimited,'>')) // If output redirection
			{
				file = freopen(redirect_filename,"a",stdout);								// Redirect output to specified file
				if(!file)																   // If file does not exist...
				{
					printf("!!! ERROR -- Freopen output redirection failed. !!!\n");	 // Indicate that output redirection failed
					freopen("/dev/tty","a",stdout);										// Reassociate stdout with the console
					continue;														   // Prompt for next command.
				}

				redirect_type = 0;													// If redirection successful, set redirect type to indicate output redirection
			}
			else if(strchr(undelimited,'<')) //Else if input redirection
			{
				file = freopen(redirect_filename,"r",stdin);
				if(!file)
				{
					printf("!!! ERROR -- File for redirection does not exist. !!!\n");
					freopen("/dev/tty","r",stdin);
					continue;
				}

				redirect_type = 1;
			}
			
			arguments[0] = strtok(redirect_command,delimiters);			   // Tokenize arguments as you would normally

			while(arguments[num_of_args] != NULL)						 // While there are still arguments/agruments left to be read...
			{
				num_of_args++;										   // Increment the total number of arguments
				arguments[num_of_args] = strtok(NULL,delimiters);	  // and read in the next argument.
			}

			redirect_flag = 1;
		}
		else 																 // Else, treat like any other command
		{
			arguments[0] = strtok(input,delimiters);					   // Tokenize first argument into char* arguments[30]

			while(arguments[num_of_args] != NULL)						 // While there are still arguments/agruments left to be read...
			{
				num_of_args++;										   // Increment the total number of arguments
				arguments[num_of_args] = strtok(NULL,delimiters);	  // and read in the next argument.
			}
		}

	   // Argument Processing
		if(strcmp(arguments[0],"exit") == 0)
		{
			exit(0);
		}
		else if(strcmp(arguments[0],"cd") == 0)			     				  // If cd command inputted...
		{
			if(chdir(arguments[1]) != 0)								  	// Check if command path is valid.
			{
				printf("!!! ERROR -- Path not found. !!!\n");		  	  // If not, indicate that the path was not found
				continue;												 // and exit with errors.
			}
		}
		else if(strcmp(arguments[0],"time") == 0)
		{

			struct tms exe_time;												   	 // Holds the user and system times of the command execution.

			if(fork() == 0)													   	   // If current process is a child...
			{
				if(execvp(arguments[1],&arguments[1]) < 0)						 // Run the command. If execvp cannot run the command...
				{
					printf("!!! ERROR -- Command was unsuccessful. !!!\n");	   // Indiciate that the command was unsuccessful
					exit(1);												  // and exit with errors.
				}
			}
			else
			{
				int status;
				wait(&status);
				
				if(times(&exe_time) < 0)														  // If time request fails...
				{
					printf("!!! ERROR -- Time fetch was unsuccessful. !!!\n");				    // Indicate that the request was unsuccessful.
				}
				else																		  // TEST CASE: Otherwise, print the contents of the exe_time tms structure
				{
					printf("User Time: %Lf\n", (long double) exe_time.tms_cutime);
					printf("Sys Time:  %Lf\n", (long double) exe_time.tms_cstime);
				}
			}

		}
		else
		{
			if(fork() == 0)															// If child process...
			{
				if(execvp(arguments[0],arguments) < 0)							  // Run the command. If execvp cannot run the command...
				{
					printf("!!! ERROR -- Command was unsuccessful. !!!\n");		// Indicate that the command was unsuccessful
					exit(1);												   // and exit with errors.
				}		
			}
			else
			{
				int status;
				wait(&status);											  // Wait for child process to complete.
			}
		}

		if(redirect_flag == 1)												// If i/o has been redirected...
		{
			fclose(file);												  // Close the associated freopen file pointer
			if(redirect_type == 0) freopen("/dev/tty","a",stdout);		 // If output redirected, reassociate stdout with console
			if(redirect_type == 1) freopen("/dev/tty","r",stdin);       // If input redirected, reassociate stdin with console
			redirect_flag = 0;										   // Set redirect flag back to 0
		}

		num_of_args = 0;											// Reinitialize number of arguments inputted

	}

	return 0;

}