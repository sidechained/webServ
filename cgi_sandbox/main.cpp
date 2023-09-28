#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    // Specify the path to the PHP interpreter and the PHP script
    const char* phpInterpreter = "/usr/bin/php"; // Replace with the actual path to php if needed
    const char* phpScript = "./test.php"; // Replace with the path to your PHP script

    // Build the argument vector
    const char* args[] = {
        phpInterpreter,
        phpScript,
        nullptr
    };

    // Define your custom environmental variables
    char fileNameVar[] = "FILENAME=output.txt";

    // Execute the PHP script with custom environmental variables set in C++
    char* const envVars[] = { fileNameVar, nullptr };

    // Create a pipe for communication
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // Fork a child process
    pid_t child_pid = fork();

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
        /*char buffer[256];
        ssize_t bytes_read;
        while ((bytes_read = read(0, buffer, sizeof(buffer))) > 0) {
            write(1, buffer, bytes_read); // Print to stdout
        }*/

        // Execute the PHP script within the child process
        if (execve(phpInterpreter, const_cast<char* const*>(args), envVars) == -1) {
            perror("execve");
            exit(EXIT_FAILURE);
        }
    } else {
		// This is the parent process
        close(pipefd[0]); // Close the read end of the pipe in the parent
        
        // Send a string to the child process
        const char* message = "Hello, child process!\n";
        write(pipefd[1], message, strlen(message));
        close(pipefd[1]);

        // Wait for the child process to finish
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
        }
    }

    return 0;
}
