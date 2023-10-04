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
    std::cout << BG_YELLOW << "Create form response"  RESET << std::endl;
    //std::cout << request.getBody() << std::endl;
    const char *phpInterpreter = "/usr/bin/php";
    std::string relativeScriptFolder = "./scripts"; // Modify this as needed
    char absoluteScriptFolder[SCRIPT_PATH_MAX];
    if (realpath(relativeScriptFolder.c_str(), absoluteScriptFolder) == NULL)
    {
        perror("realpath");
        return;
    }
    std::string absoluteScriptFolderStr = absoluteScriptFolder + request.getResource();
    const char *phpScript = absoluteScriptFolderStr.c_str();
    //std::cout << absoluteScriptFolder << std::endl;
    //std::cout << phpScript << std::endl;
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
    child_pid = fork();
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
        if (chdir(absoluteScriptFolder) != 0)
        {
            perror("chdir");
            return;
        }
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

