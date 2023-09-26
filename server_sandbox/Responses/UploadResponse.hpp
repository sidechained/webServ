#include "ErrResponse.hpp"
#include "../Requests/HttpRequest.hpp"

class UploadResponse : public ErrResponse
{
private:
    /* data */
public:
    UploadResponse(HttpRequest &request);
    ~UploadResponse();
    void createResponse(HttpRequest &request);
};

