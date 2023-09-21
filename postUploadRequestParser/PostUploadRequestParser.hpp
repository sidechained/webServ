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

struct ContentDisposition {
	std::string type;
	std::string name;
	std::string filename;
};

struct Part {
	std::map<std::string, std::string> headers;
	ContentDisposition contentDisposition;
	std::string data;
};
struct PostUploadRequest {
	std::string method;
	std::string resource;
	std::string httpVersion;
	std::map<std::string, std::string> headers;
	std::vector<Part> parts;
};
class PostUploadRequestParser
{
	private:
		PostUploadRequest postUploadRequest;
		std::string _outputFilename;
		std::vector<std::string> headerAcceptedFields;
		std::vector<std::string> partsAcceptedFields;
		std::string boundary;
		std::vector<std::string> acceptedFields;
		void initAcceptedFields();
		void parsePostUploadRequest(const std::string& requestLine, PostUploadRequest& PostUploadRequest);
		bool isAcceptedField(std::string field, std::vector<std::string> acceptedFields);
		void parseHeaderLine(const std::string& header, std::map<std::string, std::string> &headers, std::vector<std::string> acceptedFields);
		void parseHeaderBlock(std::fstream &requestFile, std::string &line, std::map<std::string, std::string> &headers, std::vector<std::string> acceptedFields);
		void parseHeaderBlockFromString(std::string& inputString, std::string& line, std::map<std::string, std::string> &headers, std::vector<std::string> acceptedFields);
		void parseContentTypeValue();
		void parseBody(std::fstream &requestFile);
		void parseContentDisposition(Part &part);
		bool isContentOfType(std::map<std::string, std::string> headers, std::string typeToMatch);
		bool arePartContentsOfType(std::string typeToMatch);
		void checkContentTypes();
		void makeOutputFile();
		// utility functions:
		bool isValidMethod(const std::string& method);
		bool isValidResource(const std::string& resource);
		bool isValidHttpVersion(const std::string& httpVersion);
		bool isValidNonNegativeIntegerString(const std::string& str);
		void toLowerCase(std::string& str);
		void splitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& output);
		void trimWhitespace(std::string& str);
		std::string stripDoubleQuotes(const std::string& str);
		bool startsWith(const std::string& fullString, const std::string& start);
		void removeFromStringStart(std::string& fullString, const std::string& prefix);
		void printPostUploadRequest();
		void printPartHeaders();
		void printHeaders(std::map<std::string, std::string> map);
	public:
		PostUploadRequestParser(std::string inputFile, std::string outputFile);
		~PostUploadRequestParser();
		PostUploadRequest getStruct();
};

#endif