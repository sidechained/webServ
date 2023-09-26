#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {
    char phpPath[] = "/usr/bin/php";
    char execName[] = "php";
    char fileName[] = "upload.php";  // Specify your PHP script file
    char fileToUpload[] = "fileToUpload";
    char boundary[] = "----MyBoundary123";   // Set the boundary
    char requestMethod[] = "REQUEST_METHOD=POST";

    char* argv[5];
    argv[0] = execName;
    argv[1] = fileName;
    argv[2] = fileToUpload;  // File field name argument
    argv[3] = boundary;
    argv[4] = NULL;

    char* envp[2];
    envp[0] = requestMethod;
    envp[1] = NULL;

    // Construct the multipart/form-data request body
    std::string requestBody = "$_FILES = ['myfile' => ['name' => 'example.txt']];";

    // Create a pipe to write the request body to stdin
    int stdin_pipe[2];
    pipe(stdin_pipe);

    pid_t pid = fork();
    if (pid == 0) {  // Child process
        close(stdin_pipe[1]);  // Close write end

        // Set stdin to read from the pipe
        dup2(stdin_pipe[0], STDIN_FILENO);
        close(stdin_pipe[0]);

        // Execute PHP script with arguments
        execve(phpPath, argv, envp);
        perror("execve");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // Parent process
        close(stdin_pipe[0]);  // Close read end

        // Write the request body to the pipe
        write(stdin_pipe[1], requestBody.c_str(), requestBody.length());
        close(stdin_pipe[1]);  // Close write end

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
