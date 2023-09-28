#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t child_pid;
    
    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }
    
    // Create a child process
    child_pid = fork();
    
    if (child_pid == -1) {
        perror("fork");
        return 1;
    }
    
    if (child_pid == 0) {
        // This is the child process
        close(pipefd[1]); // Close the write end of the pipe in the child
        
        // Redirect stdin to the read end of the pipe
        dup2(pipefd[0], 0);
        close(pipefd[0]);
        
        // Read from stdin and print to console
        char buffer[256];
        ssize_t bytes_read;
        while ((bytes_read = read(0, buffer, sizeof(buffer))) > 0) {
            write(1, buffer, bytes_read); // Print to stdout
        }
        
        return 0;
    } else {
        // This is the parent process
        close(pipefd[0]); // Close the read end of the pipe in the parent
        
        // Send a string to the child process
        const char* message = "Hello, child process!\n";
        write(pipefd[1], message, strlen(message));
        close(pipefd[1]);
        
        // Wait for the child process to complete
        wait(NULL);
    }
    
    return 0;
}
