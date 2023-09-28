#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int input_pipefd[2];  // Pipe for sending input to child
    int output_pipefd[2]; // Pipe for receiving output from child
    pid_t child_pid;
    
    // Create the input pipe
    if (pipe(input_pipefd) == -1) {
        perror("pipe");
        return 1;
    }
    
    // Create the output pipe
    if (pipe(output_pipefd) == -1) {
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
        close(input_pipefd[1]);  // Close the write end of the input pipe in the child
        close(output_pipefd[0]); // Close the read end of the output pipe in the child
        
        // Redirect stdin to the read end of the input pipe
        dup2(input_pipefd[0], 0);
        close(input_pipefd[0]);
        
        // Redirect stdout to the write end of the output pipe
        dup2(output_pipefd[1], 1);
        close(output_pipefd[1]);
        
        // Read from stdin and modify the string before printing to stdout
        char buffer[256];
        ssize_t bytes_read;
        while ((bytes_read = read(0, buffer, sizeof(buffer))) > 0) {
            // Modify the received string
            std::string modified_string(buffer, bytes_read);
            modified_string += " (modified by child)\n";
            
            // Print the modified string to stdout
            write(1, modified_string.c_str(), modified_string.length());
        }
        
        return 0;
    } else {
        // This is the parent process
        close(input_pipefd[0]);  // Close the read end of the input pipe in the parent
        close(output_pipefd[1]); // Close the write end of the output pipe in the parent
        
        // Send a string to the child process
        const char* message = "Hello, child process!\n";
        write(input_pipefd[1], message, strlen(message));
        close(input_pipefd[1]);
        
        // Read the output from the child process and print it
        char output_buffer[256];
        ssize_t output_bytes_read;
        while ((output_bytes_read = read(output_pipefd[0], output_buffer, sizeof(output_buffer))) > 0) {
            write(1, output_buffer, output_bytes_read); // Print to stdout
        }
        close(output_pipefd[0]);
        
        // Wait for the child process to complete
        wait(NULL);
    }
    
    return 0;
}
