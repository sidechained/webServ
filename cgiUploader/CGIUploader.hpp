#ifndef CGIUPLOADER_HPP
# define CGIUPLOADER_HPP

# include <cstdlib>
# include <unistd.h>
# include "../configFileParser/ConfigFileParser.hpp"
# include "../postUploadRequestParser/PostUploadRequestParser.hpp"

class CGIUploader {
	private:
		std::string _dataOutputFilename;
		void prepareEnvironment(PostUploadRequest &postUploadRequest);
		int executePhpScript(const char *filePath);
		unsigned int stringToUnsignedInt(const std::string& str);
	public:
		CGIUploader(ServerConfig &serverConfig, std::string postRequestFilename, std::string dataOutputFilename);
		~CGIUploader();
};

#endif