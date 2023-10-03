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
    PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Path: " << request.getPath());
    if (request.getLocationConfig()->redirection != "")
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
    if (request.getLocationConfig()->autoindex && !request.isFile())
    {
        //if resource is a file continue
        std::cout << BG_BOLD_MAGENTA << "Autoindexing" << RESET << std::endl;
        std::cout << BG_YELLOW << "resource:" << request.getResource() << RESET << std::endl;
        createAutoIndexResponse(request.getPath(), request.getResource());
        setHeader(OkHeader(request.getContentType(), this->getBodyLength()).getHeader());
        return;
    }
    std::ifstream htmlFile(request.getPath().c_str(), std::ios::binary);
    if (!htmlFile)
    {
        std::string error = "404";
        this->createErrResponse(error);
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

void TextResponse::createAutoIndexResponse(std::string dirName, std::string resource)
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

    genDir(dirName, resource);

    _body += "\
	</p>\n\
	</body>\n\
	</html>\n";
    setBody(_body);
}

void TextResponse::genDir(std::string dirName, std::string resource)
{
    // add ./ to dirName
    std::string dirNameStr = std::string(dirName);
    if (dirNameStr[0] != '.')
        dirNameStr = "./" + dirNameStr;
    // remove file name from dirNameStr
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
        genLink(std::string(entry->d_name), resource);
    }
    closedir(dir);
}

void TextResponse::genLink(std::string entryName, std::string resource)
{
    std::string host = _request->getHost();
    if (resource[0] == '.')
        resource = resource.substr(1);
    if (resource[0] == '/')
        resource = resource.substr(1);

    std::cout << BG_BOLD_RED << "genLink" << std::endl;
    std::cout << "host: " << host << std::endl;
    std::cout << "resource: " << resource << std::endl;
    std::cout << "entryName: " << entryName << std::endl;

    // std::stringstream ss;
    // ss << port;
    _body += "\t\t<p><a href=\"http://" + host + "/" + resource + entryName + "\">" + entryName + "</a></p>\n";
}
