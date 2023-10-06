#include "FormResponse.hpp"

FormResponse::FormResponse(HttpRequest &request) : ErrResponse(request)
{
    _cgi = true;
    initPipe();
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
        return;
    }

    // Create the output pipe
    if (pipe(output_pipefd) == -1)
    {
        perror("pipe");
        return;
    }
}

void FormResponse::createResponse(HttpRequest &request)
{
    _cgi = true;
    std::cout << BG_YELLOW << "Create form response" RESET << std::endl;
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

    //  Build the argument vector
    const char *args[] = {
        phpInterpreter,
        phpScript,
        NULL};

    std::vector<std::string> envVarsVect = request.getEnvVars();
    std::vector<const char *> envVars(envVarsVect.size() + 1, NULL);

    for (size_t i = 0; i < envVarsVect.size(); i++)
    {
        // Assign the c_str() of the strings to the vector of const char*
        envVars[i] = envVarsVect[i].c_str();
        std::cout << "envVars[" << i << "]: " << envVarsVect[i] << std::endl;
    }

    // Ensure the last element is NULL to terminate the array
    envVars[envVarsVect.size()] = NULL;

    // Execute the PHP script with custom environmental variables set in C++
    // char *const envVars[] = {NULL};
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
        if (execve(phpInterpreter, const_cast<char *const *>(args), const_cast<char *const *>(envVars.data())) == -1)
        {
            // Handle the execve error
            perror("execve");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        close(input_pipefd[0]);  // Close the read end of the input pipe in the parent
        close(output_pipefd[1]); // Close the write end of the output pipe in the parent

        // Record the current system time before starting the child process
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
