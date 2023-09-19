#ifndef POSTUPLOADREQUESTPARSER_HPP
# define POSTUPLOADREQUESTPARSER_HPP

# include <cstdlib>
# include <fstream>
# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>

class PostUploadRequestParser
{
	private:
		struct HttpRequest {
			std::string method;
			std::string resource;
			std::string httpVersion;
			std::string host;
			std::string userAgent;
			std::string accept;
			std::string connection;
		};
		std::vector<std::string> acceptedFields;
		bool isValidMethod(const std::string& method);
		bool isValidResource(const std::string& resource);
		bool isValidHttpVersion(const std::string& httpVersion);
		void parseHttpRequest(const std::string& requestLine, HttpRequest& httpRequest);
		void parseHeaderBlock(std::fstream &requestFile, std::string &line);
		bool isAcceptedField(std::string field);
		void parseHeaderLine(const std::string& header, std::map<std::string, std::string>& headerMap);
		void printHeaderMap(std::map<std::string, std::string> headerMap);
	public:
		PostUploadRequestParser();
		~PostUploadRequestParser();
};

#endif