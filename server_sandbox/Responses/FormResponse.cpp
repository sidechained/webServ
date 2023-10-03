#include "FormResponse.hpp"

FormResponse::FormResponse(HttpRequest &request) : ErrResponse(request)
{
	_cgi = true;
	initPipe();
    //createResponse(request);
}

FormResponse::~FormResponse()
{
}

void FormResponse::initPipe()
{
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
}

void FormResponse::createResponse(HttpRequest &request)
{
	_cgi = true;
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
	// Get the boundary string from request
	std::string strBoundary = "BOUNDARY=" + request.getBoundary();
	removeNonPrintableChars(strBoundary);
	std::cout << BG_RED << strBoundary << RESET << std::endl;

	// Convert the boundary string to a mutable character array
	char boundary[strBoundary.size() + 1]; // +1 for null terminator
	strcpy(boundary, strBoundary.c_str());

	std::string strUploadPath = "UPLOAD_PATH=." + request.getLocationConfig()->uploads;
	removeNonPrintableChars(strUploadPath);
	std::cout << BG_RED << strUploadPath << RESET << std::endl;

	// Convert the boundary string to a mutable character array
	char uploadPath[strUploadPath.size() + 1]; // +1 for null terminator
	strcpy(uploadPath, strUploadPath.c_str());

    // Execute the PHP script with custom environmental variables set in C++
    char *const envVars[] = {fileNameVar, boundary, uploadPath,  NULL};

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

       /* // std::string body = request.getBody();
        // const char* message = body.c_str();
        std::vector<char> body = request.getBodyVector();
    //write body to input pipe
        for (std::vector<char>::iterator it = body.begin(); it != body.end(); ++it)
        {
            write(input_pipefd[1], &(*it), 1);
        }
        // write(input_pipefd[1], message, strlen(message));
        close(input_pipefd[1]);*/



		/*std::vector<char> htmlOutputBuffer;  // Dynamic buffer to store the HTML output

		// Read the output from the child process and store it in htmlOutputBuffer
		char output_buffer[256];
		ssize_t output_bytes_read;
		while ((output_bytes_read = read(output_pipefd[0], output_buffer, sizeof(output_buffer))) > 0) {
			htmlOutputBuffer.insert(htmlOutputBuffer.end(), output_buffer, output_buffer + output_bytes_read);
		}
		close(output_pipefd[0]);


		// Convert the vector to a string
		std::string htmlString(htmlOutputBuffer.begin(), htmlOutputBuffer.end());
		setBody(htmlString);
		//_body = htmlString;

		setHeader(OkHeader(this->getRequest().getContentType(), this->getBodyLength()).getHeader());

		std::cout << BG_BLUE << _response << RESET << std::endl;*/


		

         // Wait for the child process to finish
		PRINT(FORMRESPONSE, BG_YELLOW, "Waiting for child process to finish")
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
        }
    }
}

int FormResponse::getInputPipefd()
{
	return input_pipefd[1];
}

int FormResponse::getOutputPipefd()
{
	return output_pipefd[0];
}

