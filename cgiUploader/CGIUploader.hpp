#ifndef CGIUPLOADER_HPP
# define CGIUPLOADER_HPP



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
		CGIUploader();
		~CGIUploader();
		receiveFileUploadRequest();
		unchunk();
		prepData();
		runScript();
		sendResponse();
	public:
};

#endif