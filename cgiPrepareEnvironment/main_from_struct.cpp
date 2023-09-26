// no makefile yet so run this:
// clear; g++ -Wall -Werror -Wextra -std=c++98 ../postUploadRequestParser/PostUploadRequestParser.cpp main_from_struct.cpp

#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string>
#include "../postUploadRequestParser/PostUploadRequestParser.hpp"

// 1. set REQUEST_METHOD, FILE_NAME, FILE_TMP_NAME and FILE_ERROR environment variables (these will appear to php in #_SERVERS superglobal)
// - FILE_NAME, FILE_TMP_NAME and FILE_ERROR will be put into files in the first line of the php script
// 2. provide "upload.php" script as argument to php args

int main() {

    std::string tmpName = "resources/postRequestData.txt";
    PostUploadRequestParser purp("resources/postRequest.txt", tmpName);
    // note: postRequestData will be three 'parts' chunked together, but this should be changed to be three separate files

    std::cout << purp.getStruct().method << std::endl;

    char* argv[] = {
        (char*)"/usr/bin/php",
        (char*)"resources/upload.php",
        NULL
    };

    // Create a string for the combined request method
    // note: we are taking everything from the first part for now i.e. part[0] 
    // note: do we need 'name'?
    // std::cout << purp.getStruct().parts[0].contentDisposition.name << std::endl;

    std::string requestMethodEnvVar = "REQUEST_METHOD=" + purp.getStruct().method;
    std::string filenameEnvVar = "FILE_NAME=" + purp.getStruct().parts[0].contentDisposition.filename;
    std::string fileTmpNameEnvVar = "FILE_TMP_NAME=" + tmpName;
    
    // Convert the combinedRequestMethod to a char* for envp
    char* envp[] = {
        const_cast<char*>(requestMethodEnvVar.c_str()),
        const_cast<char*>(filenameEnvVar.c_str()),
        const_cast<char*>(fileTmpNameEnvVar.c_str()),
        (char*)"FILE_ERROR=0", // do we even need to generate this - remember it is hardcoded in the script for now
        NULL
    };

    std::cout << envp[0] << std::endl;

    // open request body
    std::string data = "This is the request body data\n";

    // Create a pipe to write the request body to stdin
    int stdin_pipe[2];
    pipe(stdin_pipe);

    pid_t pid = fork();
    if (pid == 0) {  // Child process
        execve("usr/bin/php", argv, envp);
        perror("execve");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // Parent process
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}