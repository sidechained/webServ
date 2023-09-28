#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include "Colors.hpp"
#include <fstream>

int main() {

	// Step 1: Create an instance of ofstream to work with files.
    std::ofstream outputFile;

    // Step 2: Open the file. This will create the file if it doesn't exist.
    outputFile.open("output.txt");

    // Step 3: Check if the file is successfully opened.
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1; // Exit with an error code.
    }

    // Step 4: Write "Hello, world!" to the file.
    outputFile << "Hello, world!" << std::endl;

    // Step 5: Close the file when you're done with it.
    outputFile.close();

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
    int input_pipefd[2];  // Pipe for sending input to child
    int output_pipefd[2]; // Pipe for receiving output from child

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

    // Fork a child process
    pid_t child_pid = fork();

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
        /*char buffer[256];
        ssize_t bytes_read;
        while ((bytes_read = read(0, buffer, sizeof(buffer))) > 0) {
            // Modify the received string
            std::string modified_string(buffer, bytes_read);
            modified_string += " (modified by child)\n";
            
            // Print the modified string to stdout
            write(1, modified_string.c_str(), modified_string.length());
        }*/

        // Execute the PHP script within the child process
        if (execve(phpInterpreter, const_cast<char* const*>(args), envVars) == -1) {
            perror("execve");
            exit(EXIT_FAILURE);
        }
    } else {
		// This is the parent process
        close(input_pipefd[0]);  // Close the read end of the input pipe in the parent
        close(output_pipefd[1]); // Close the write end of the output pipe in the parent
        
        // Send a string to the child process
        const char* message = "Hello, child process!\n";
        write(input_pipefd[1], message, strlen(message));
        close(input_pipefd[1]);

		std::cout << BOLD_GREEN << "Output from PHP script:" << RESET << std::endl;
        // Read the output from the child process and print it
        char output_buffer[256];
        ssize_t output_bytes_read;
        while ((output_bytes_read = read(output_pipefd[0], output_buffer, sizeof(output_buffer))) > 0) {
            write(1, output_buffer, output_bytes_read); // Print to stdout
        }
        close(output_pipefd[0]);
		std::cout << BOLD_GREEN << "End of output from PHP script" << RESET << std::endl;

        // Wait for the child process to finish
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
        }
    }

    return 0;
}
