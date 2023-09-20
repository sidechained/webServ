#ifndef POSTUPLOADREQUESTPARSER_HPP
# define POSTUPLOADREQUESTPARSER_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
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
		std::map<std::string, std::string> headerMap;
		std::vector<std::string> headerAcceptedFields;
		std::vector<std::string> partsAcceptedFields;
		std::string boundary;
		std::vector<std::string> acceptedFields;
		bool isValidMethod(const std::string& method);
		bool isValidResource(const std::string& resource);
		bool isValidHttpVersion(const std::string& httpVersion);
		void parseHttpRequest(const std::string& requestLine, HttpRequest& httpRequest);
		bool isAcceptedField(std::string field, std::vector<std::string> acceptedFields);
		void parseHeaderLine(const std::string& header, std::map<std::string, std::string> &headerMap, std::vector<std::string> acceptedFields);
		void parseHeaderBlock(std::fstream &requestFile, std::string &line, std::map<std::string, std::string> &headerMap, std::vector<std::string> acceptedFields);
		void parseHeaderBlockFromString(std::string& inputString, std::string& line, std::map<std::string, std::string>& headerMap, std::vector<std::string> acceptedFields);
		void parseContentTypeValue();
		void parseBody(std::fstream &requestFile);
		void toLowerCase(std::string& str);
		void splitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& output);
		void trimWhitespace(std::string& str);
		bool startsWith(const std::string& fullString, const std::string& start);
		void removeFromStringStart(std::string& fullString, const std::string& prefix);
		void printHttpRequest(HttpRequest& httpRequest);
		void printMap(std::map<std::string, std::string> map);
	public:
		PostUploadRequestParser();
		~PostUploadRequestParser();
};

#endif