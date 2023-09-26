#include "TextResponse.hpp"

TextResponse::TextResponse(HttpRequest &request) : ErrResponse(request)
{
    createResponse(request);
}

TextResponse::~TextResponse()
{
}

void TextResponse::createResponse(HttpRequest &request)
{
    PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Creating HTML response for resource");
    request.printRequest();
    PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Path: " << request.getPath());
    if (request.getRedirection() != "")
    {
        std::cout << BG_BOLD_MAGENTA << "Redirection in resource" << RESET << std::endl;
        setHeader(MovedHeader(request.getHost(), request.getRedirection()).getHeader());
        return;
    }

    if (request.getError("methodNotAllowed"))
    {
        std::cout << BG_BOLD_MAGENTA << "Method not allowed" << RESET << std::endl;
        std::string error = "405";
        this->createErrResponse(error);
        return;
    }
    std::ifstream htmlFile(request.getPath().c_str(), std::ios::binary);
    if (!htmlFile)
    {
        std::string error = "404";
        this->createErrResponse(error);
        return;
    }
    if (request.getAutoIndex())
    {
        std::cout << BG_BOLD_MAGENTA << "Autoindexing" << RESET << std::endl;
        createAutoIndexResponse(request.getPath().c_str());
        setHeader(OkHeader(request.getContentType(), this->getBodyLength()).getHeader());
        std::cout << "header: " << this->getHeader() << std::endl;
        std::cout << "body: " << this->getBody() << std::endl;
        return;
    }

    setBody(htmlFile);
    setHeader(OkHeader(this->getRequest().getContentType(), this->getBodyLength()).getHeader());

    // PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Header: " << this->getHeader().substr(0, 1000))
    // PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Body: " << this->getBody().substr(0, 100))

    htmlFile.clear();
    htmlFile.seekg(0, std::ios::beg);
    htmlFile.close();
}

void TextResponse::createAutoIndexResponse(const char *dirName)
{

    _body =
        "<!DOCTYPE html>\n\
	<html>\n\
	<head>\n\
			<title>" +
        std::string(dirName) + "</title>\n\
	</head>\n\
	<body>\n\
	<h1>INDEX</h1>\n\
	<p>\n";

    genDir(dirName);

    _body += "\
	</p>\n\
	</body>\n\
	</html>\n";
    setBody(_body);
}

void TextResponse::genDir(const char *dirName)
{
    // add ./ to dirName
    std::string dirNameStr = std::string(dirName);
    if (dirNameStr[0] != '.')
        dirNameStr = "./" + dirNameStr;
    //remove file name from dirNameStr
    size_t lastSlash = dirNameStr.find_last_of("/");
    dirNameStr = dirNameStr.substr(0, lastSlash);
    std::cout << "dirName: " << dirNameStr << std::endl;
    DIR *dir = opendir(dirNameStr.c_str());
    if (!dir)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        genLink(std::string(entry->d_name), std::string(dirNameStr));
    }
    closedir(dir);
}

void TextResponse::genLink(std::string entryName, std::string dirName)
{
    std::string host = _request->getHost();
    std::cout << "host: " << host << std::endl;
    if (dirName[0] == '.')
        dirName = dirName.substr(1);
    // std::stringstream ss;
    // ss << port;
    _body += "\t\t<p><a href=\"http://" + host  + dirName + "/" + entryName + "\">" + entryName + "</a></p>\n";
}
