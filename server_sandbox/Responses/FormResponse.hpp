#include "ErrResponse.hpp"
#include "../Requests/HttpRequest.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "../WebServ.hpp"


class FormResponse : public ErrResponse
{
private:
    int input_pipefd[2];  // Pipe for sending input to child
    int output_pipefd[2]; // Pipe for receiving output from child
public:
    FormResponse(HttpRequest &request);
    ~FormResponse();
    void createResponse(HttpRequest &request);
};
