#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>

// void prepareCGIEnvironment() {

    // char contentType[] = "CONTENT_TYPE=multipart/form-data";
    // char contentLength[50];  // Assuming a reasonable size for the content length
    // snprintf(contentLength, sizeof(contentLength), "CONTENT_LENGTH=%lu", uploadDir.size());
    // char documentRoot[50];  // Assuming a reasonable size for the document root
    // snprintf(documentRoot, sizeof(documentRoot), "DOCUMENT_ROOT=%s", uploadDir.c_str());
    // char scriptFilename[50];  // Assuming a reasonable size for the script filename
    // snprintf(scriptFilename, sizeof(scriptFilename), "SCRIPT_FILENAME=%supload.php", uploadDir.c_str());
    // putenv(contentType);
    // putenv(contentLength);
    // putenv(documentRoot);
    // putenv(scriptFilename);
// }

// DOCUMENT_ROOT is populated anyway and is empty and overwrites whatever we might try to do below
int main() {
    std::string uploadDir = "uploads/";
    char* argv[] = {const_cast<char*>("php"), const_cast<char*>("upload.php"), NULL};
    char* envp[5];
    char requestMethod[] = "REQUEST_METHOD=POST";
    char contentType[] = "CONTENT_TYPE=multipart/form-data";
    char contentLength[] = "CONTENT_LENGTH=1234";
    //char documentRoot[] = "DOCUMENT_ROOT=/nfs/homes/gbooth/42cursus/5_3_webserv_repo/cgiUploader"; // full path
    envp[0] = requestMethod;
    envp[1] = contentType;
    envp[2] = contentLength;
    // envp[3] = documentRoot;
    envp[3] = NULL;
    execve("/usr/bin/php", argv, envp);
    perror("execve");
    return 1;
}