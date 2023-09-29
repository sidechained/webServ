#include "FormResponse.hpp"

FormResponse::FormResponse(HttpRequest &request) : ErrResponse(request)
{
    createResponse(request);
}

FormResponse::~FormResponse()
{
}

void FormResponse::createResponse(HttpRequest &request)
{
    std::cout << BG_YELLOW << "Create form response" << std::endl;
    std::cout << request.getBody() << std::endl;

    const char *phpInterpreter = "/usr/bin/php";
    std::string resource = "./scripts" + request.getResource(); // Make a copy of the string
    const char *phpScript = resource.c_str();

    // Build the argument vector
    const char *args[] = {
        phpInterpreter,
        phpScript,
        NULL};

    // Define your custom environmental variables
    char fileNameVar[] = "REQUEST_METHOD=POST";

    // Execute the PHP script with custom environmental variables set in C++
    char *const envVars[] = {fileNameVar, NULL};

    // Create the input pipe
    if (pipe(input_pipefd) == -1)
    {
        perror("pipe");
        // fill error response
        return;
    }

    // Create the output pipe
    if (pipe(output_pipefd) == -1)
    {
        perror("pipe");
        // fill error response
        return;
    }

    // Fork a child process
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        perror("fork");
        // fill error response
        return;
    }
    if (child_pid == 0)
    {
        // This is the child process
        close(input_pipefd[1]);  // Close the write end of the input pipe in the child
        close(output_pipefd[0]); // Close the read end of the output pipe in the child

        // Redirect stdin to the read end of the input pipe
        dup2(input_pipefd[0], 0);
        close(input_pipefd[0]);

        // Redirect stdout to the write end of the output pipe
        dup2(output_pipefd[1], 1);
        close(output_pipefd[1]);

        // Execute the PHP script within the child process
        if (execve(phpInterpreter, const_cast<char *const *>(args), envVars) == -1)
        {
            perror("execve");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        close(input_pipefd[0]);  // Close the read end of the input pipe in the parent
        close(output_pipefd[1]); // Close the write end of the output pipe in the parent

        std::string body = request.getBody();
        const char* message = body.c_str();
        write(input_pipefd[1], message, strlen(message));
        close(input_pipefd[1]);

        		// Create a file for writing the HTML output
		std::ofstream htmlFile("output.html");

		if (!htmlFile) {
			std::cerr << "Failed to open HTML output file." << std::endl;
			return ;
		}

		std::vector<char> htmlOutputBuffer;  // Dynamic buffer to store the HTML output

		// Read the output from the child process and store it in htmlOutputBuffer
		char output_buffer[256];
		ssize_t output_bytes_read;
		while ((output_bytes_read = read(output_pipefd[0], output_buffer, sizeof(output_buffer))) > 0) {
			htmlOutputBuffer.insert(htmlOutputBuffer.end(), output_buffer, output_buffer + output_bytes_read);
		}
		close(output_pipefd[0]);

		// Write the HTML output to the file
		htmlFile.write(htmlOutputBuffer.data(), htmlOutputBuffer.size());

		// Close the HTML file
		htmlFile.close();
        //save htmlOutputBuffer to body
         // Wait for the child process to finish
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
        }
    }
}