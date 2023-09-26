#include <iostream>
#include <cstring>
#include <unistd.h>

int main() {
    // Set up the environment variables
    char* envp[] = {
        (char*)"CONTENT_TYPE=multipart/form-data; boundary=---------------------------1234567890",
        (char*)"CONTENT_LENGTH=289",  // Adjust the content length as needed
        nullptr  // Terminate the array with a nullptr
    };

    // Set up the command-line arguments
    char* argv[] = {
        (char*)"php",
        (char*)"-r",
        (char*)"$_FILES = array('myfile' => array('name' => 'example.txt', 'tmp_name' => '/tmp/php1234', 'type' => 'text/plain', 'size' => 12345));"
        "print_r($_FILES);",
        nullptr  // Terminate the array with a nullptr
    };

    // Execute PHP using execve
    execve("/usr/bin/php", argv, envp);

    // If execve fails, print an error message
    perror("execve");
    return 1;
}
