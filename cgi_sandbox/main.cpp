
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>

int main() {
    // Specify the file name
    const char* fileName = "output.txt";

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
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
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
        // This code is executed by the child process

        // Close the read end of the pipe (child writes to parent)
        close(pipe_fd[0]);

        // Duplicate the write end of the pipe to stdout (file descriptor 1)
        dup2(pipe_fd[1], 1);



        // Execute the PHP script within the child process
        if (execve(phpInterpreter, const_cast<char* const*>(args), envVars) == -1) {
            perror("execve");
            exit(EXIT_FAILURE);
        }
    } else {
        // This code is executed by the parent process

        // Close the write end of the pipe (parent reads from child)
        close(pipe_fd[1]);

        // Read data from the pipe (output of the child process)
        char buffer[4096];
        ssize_t bytes_read;

        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
            // Process the data read from the child process
            // For example, you can write it to a file or display it
            write(STDOUT_FILENO, buffer, bytes_read);
        }

        // Close the read end of the pipe
        close(pipe_fd[0]);

        // Wait for the child process to finish
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
        }
    }

    return 0;
}
