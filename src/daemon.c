#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

//TODO: add signal processing
int daemonize(char* pid_file, int (core)()){
	pid_t process_id = 0;
	pid_t sid = 0;
	// Create child process
	process_id = fork();
	// Indication of fork() failure
	if (process_id < 0){
		printf("fork failed!\n");
		// Return failure in exit status
		exit(1);
	}
	// PARENT PROCESS. Need to kill it.
	if (process_id > 0){
		printf("Process id %d \n", process_id);
		if (pid_file!=0){
			FILE* f;
			f=fopen(pid_file,"wt+");
			if (f==0)
				printf("Failed to open log: %s\n",pid_file);
			else{
				fprintf(f,"%d",process_id);
				fclose(f);
			}
		}
		// return success in exit status
		exit(0);
	}
	//unmask the file mode
	umask(0);
	//set new session
	sid = setsid();
	if(sid < 0){
		// Return failure
		exit(1);
	}
	// Change the current working directory to root.
	//check meaby need
//	chdir("/");
	// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	//some work
	core();
	
	return (0);
}