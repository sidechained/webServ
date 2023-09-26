#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string>

// 1. set REQUEST_METHOD, FILE_NAME, FILE_TMP_NAME and FILE_ERROR environment variables (these will appear to php in #_SERVERS superglobal)
// - FILE_NAME, FILE_TMP_NAME and FILE_ERROR will be put into files in the first line of the php script
// 2. provide "upload.php" script as argument to php args

int main() {
    char* argv[] = {    	
        (char*)"/usr/bin/php",
        (char*)"upload.php",
        NULL
    };

 	// eventually this will need to be autopopulated from my struct
    char* envp[] = {
        (char*)"REQUEST_METHOD=POST",
	    (char*)"FILE_NAME=example.txt",
		(char*)"FILE_TMP_NAME=temp.txt",
		(char*)"FILE_ERROR=0",
        NULL
    };

    // open request body
    std::string data = "This is the request body data\n";

    // Create a pipe to write the request body to stdin
    int stdin_pipe[2];
    pipe(stdin_pipe);

    pid_t pid = fork();
    if (pid == 0) {  // Child process
        perror("execve");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // Parent process
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

