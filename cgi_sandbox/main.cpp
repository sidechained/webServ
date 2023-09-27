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

        // Close the read end of the pipe (child doesn't read from parent)
        close(pipe_fd[0]);

        // Duplicate the write end of the pipe to stdin (file descriptor 0)
        dup2(pipe_fd[1], 0);

        // Close the write end of the pipe (no longer needed)
        close(pipe_fd[1]);

        // Execute the PHP script within the child process
        if (execve(phpInterpreter, const_cast<char* const*>(args), envVars) == -1) {
            perror("execve");
            exit(EXIT_FAILURE);
        }
    } else {
        // This code is executed by the parent process

        // Close the write end of the pipe (parent writes to child)
        close(pipe_fd[1]);

        // Write your string to the pipe (child's stdin)
        const char* message = "This is the string you want to send to PHP\n";
        ssize_t bytes_written = write(pipe_fd[0], message, strlen(message));
        if (bytes_written == -1) {
            perror("write");
            return 1;
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
