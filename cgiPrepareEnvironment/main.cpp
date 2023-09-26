#include <cstdlib>
#include <unistd.h>
#include <stdio.h>

// 1. set REQUEST_METHOD environment variable (this will appear to php in #_SERVERS superglobal)
// 2. provide "upload.php" script as argument to php args
// 3. provide $_FILES as -r argument to php args
// 4. pipe request body data to stdin of php process

int main() {
    const char* fileName = "example.txt";
    const char* tmpFilePath = "/tmp/php1234";
    const char* errorCode = "0";

    // Construct the PHP command with variables
    char phpScript[1024];
    snprintf(phpScript, sizeof(phpScript), "$_FILES = array('myfile' => array('name' => '%s', 'tmp_name' => '%s', 'error' => '%s'));\nprint_r($_FILES);", fileName, tmpFilePath, errorCode);

    char* argv[] = {
    	(char*)"upload.php",
        (char*)"/usr/bin/php",
        (char*)"-r",
        phpScript,
        NULL
    };

    char* envp[] = {
        (char*)"REQUEST_METHOD=POST",
        NULL
    };

    execve("/usr/bin/php", argv, envp);
    perror("execve"); // Print error message
    return EXIT_FAILURE;
}

