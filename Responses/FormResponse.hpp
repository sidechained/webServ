#include "ErrResponse.hpp"
#include "../Requests/HttpRequest.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "../WebServ.hpp"
#define SCRIPT_PATH_MAX 200

class FormResponse : public ErrResponse
{
private:
public:
	pid_t child_pid;
    int input_pipefd[2];  // Pipe for sending input to child
    int output_pipefd[2]; // Pipe for receiving output from child
    FormResponse(HttpRequest &request);
    ~FormResponse();
    void createResponse(HttpRequest &request);
	int getInputPipefd();
	int getOutputPipefd();
	void executeResponse();
	void initPipe();
};

