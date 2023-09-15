#include <cstdlib>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sstream>
#include <stdio.h>
#include <errno.h>

std::string getLink(std::string entryName, std::string dirName)
{
	std::string link;
	std::string host = "example.com";
	int port = 80;
	std::stringstream ss;
    ss << port;

	link = "\t\t<p><a href=\"http://" + host + ":" + ss.str() + dirName + "/" + entryName + "\">" + entryName + "</a></p>\n";
	return link;
}

void genDir(std::string &page, const char *dirName)
{
	DIR *dir = opendir(dirName);
	if (!dir) {
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		// if (entryName.compare(".") == 0 || entryName.compare("..") == 0)
		// 	continue;
		page += getLink(std::string(entry->d_name), std::string(dirName));
	}	
	closedir(dir);
}

std::string genPage(const char *dirName)
{
	std::string page =\
	"<!DOCTYPE html>\n\
	<html>\n\
	<head>\n\
			<title>" + std::string(dirName) + "</title>\n\
	</head>\n\
	<body>\n\
	<h1>INDEX</h1>\n\
	<p>\n";

	genDir(page, dirName);

	page +="\
	</p>\n\
	</body>\n\
	</html>\n";
	return page;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Program requires single arg (path to generate index.html for)" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string page;
	const char *dirPath = argv[1];
	page = genPage(dirPath);
	std::cout << page;

    std::ofstream outputFile("index.html");
    if (outputFile.is_open()) {
        outputFile << page;
        outputFile.close();
        std::cout << "Page saved to index.html" << std::endl;
    } else {
        std::cerr << "Failed to open the output file." << std::endl;
        return EXIT_FAILURE;
    }
}