#include "AutoIndexGenerator.hpp"

AutoIndexGenerator::AutoIndexGenerator(const char *dirPath)
{
	genPage(dirPath);
	writePage();
}

AutoIndexGenerator::~AutoIndexGenerator()
{

}

void AutoIndexGenerator::genLink(std::string entryName, std::string dirName)
{
	std::string host = "example.com";
	int port = 80;
	std::stringstream ss;
	ss << port;
	page += "\t\t<p><a href=\"http://" + host + ":" + ss.str() + dirName + "/" + entryName + "\">" + entryName + "</a></p>\n";
}

void AutoIndexGenerator::genDir(const char *dirName)
{
	DIR *dir = opendir(dirName);
	if (!dir) {
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		genLink(std::string(entry->d_name), std::string(dirName));
	}	
	closedir(dir);
}

void AutoIndexGenerator::genPage(const char *dirName)
{
	page =\
	"<!DOCTYPE html>\n\
	<html>\n\
	<head>\n\
			<title>" + std::string(dirName) + "</title>\n\
	</head>\n\
	<body>\n\
	<h1>INDEX</h1>\n\
	<p>\n";

	genDir(dirName);

	page += "\
	</p>\n\
	</body>\n\
	</html>\n";
}

void AutoIndexGenerator::writePage()
{
	std::ofstream outputFile("index.html");
	if (outputFile.is_open()) {
		outputFile << page;
		outputFile.close();
		std::cout << "Page saved to index.html" << std::endl;
	} else {
		std::cerr << "Failed to open the output file." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void AutoIndexGenerator::printPage()
{
	std::cout << page;
}