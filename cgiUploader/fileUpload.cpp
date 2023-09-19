// clear; c++ -Wall -Werror -Wextra -std=c++98 fileUpload.cpp && ./a.out


#include <iostream>
#include <fstream>
#include <sstream>

struct HttpRequest {
    std::string method;
    std::string resource;
    std::string httpVersion;
    std::string host;
    std::string userAgent;
    std::string accept;
    std::string connection;
};

void print_buffer(char (&buffer)[1024], ssize_t &bytesRead)
{
	std::cerr << "Received the following data from client..." << std::endl;
	std::cout.write(buffer, bytesRead);
	std::cout << std::endl;
}

int read_post_request_into_buffer(const char* filename, char (&buffer)[1024], ssize_t &bytes_read)
{
    std::ifstream postRequestFile(filename, std::ios::binary);
    if (!postRequestFile) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }
    while (!postRequestFile.eof())
    {
        postRequestFile.read(buffer, sizeof(buffer));
        bytes_read = postRequestFile.gcount();
        if (bytes_read <= 0)
            return 1;
    }
    postRequestFile.close();
    return 0;
}

bool has_given_extension(const std::string& filename, const std::string& extensionToFind) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos);
        if (extension == extensionToFind) {
            return true;
        }
    }
    return false;
}

bool parse_request(const std::string& request_str, HttpRequest& request) {
    std::istringstream requestStream(request_str);
    requestStream >> request.method >> request.resource >> request.httpVersion; // first line
    if (request.method == "POST" && request.httpVersion == "HTTP/1.1")
    {
        if (has_given_extension(request.resource, ".php"))
        {
            std::cout << "php extension detected!" << std::endl;
            // setup CGI environment
            export REQUEST_METHOD="POST";
            // execute php script
            if (execve("/usr/bin/php", {"/usr/bin/php", "cgi/upload.php", NULL}, NULL) == -1) {
                std::cerr << "Execve failed." << std::endl;
                return 1; // Handle execve failure
            }
        }

        }
        return true;
    }
    return false;
}

int main()
{
    char buffer[1024];
    ssize_t bytes_read;
    if (read_post_request_into_buffer("requests/POSTfileUpload.txt", buffer, bytes_read))
        return 0;
    // print_buffer(buffer, bytes_read);
    HttpRequest parsedRequest;
    parse_request(buffer, parsedRequest);
}