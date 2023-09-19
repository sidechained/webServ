#ifndef CGIUPLOADER_HPP
# define CGIUPLOADER_HPP

# include <cstdlib>
# include <unistd.h>
# include "../configFileParser/ConfigFileParser.hpp"

class CGIUploader {
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
		bool matchResource(const std::string& firstLine, const std::string& resourceToMatch, HttpRequest &request);
		void prepareData();
		void prepareEnvironment(HttpRequest &request);
		int executePhpScript(const char *filePath);
	public:
		CGIUploader();
		~CGIUploader();
};

#endif