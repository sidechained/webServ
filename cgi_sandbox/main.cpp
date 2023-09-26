#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

int main() {
    // Define your custom environmental variables
    char customVar1[] = "CUSTOM_VAR1=custom_value1";
    char customVar2[] = "CUSTOM_VAR2=custom_value2";

    // Set custom environmental variables in C++
    //putenv(customVar1);
    //putenv(customVar2);

    // Specify the path to the PHP interpreter and the PHP script
    const char* phpInterpreter = "/usr/bin/php"; // Replace with the actual path to php if needed
    const char* phpScript = "./test.php"; // Replace with the path to your PHP script

    // Build the argument vector
    const char* args[] = {
        phpInterpreter,
        phpScript,
        nullptr
    };

    // Execute the PHP script with custom environmental variables set in C++
    char* const envVars[] = { customVar1, customVar2, nullptr };

    if (execve(phpInterpreter, const_cast<char* const*>(args), envVars) == -1) {
        perror("execve");
        exit(EXIT_FAILURE);
    }

    return 0;
}


