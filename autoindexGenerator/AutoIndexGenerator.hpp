#ifndef AUTOINDEXGENERATOR_HPP
# define AUTOINDEXGENERATOR_HPP

# include <cstdlib>
# include <iostream>
# include <fstream>
# include <dirent.h>
# include <sstream>
# include <stdio.h>
# include <errno.h>

// this class takes a path on construction, and renders the directory listing at that path as a simple html index page, writing it to 'index.html'
// TODO: in genLink make links render properly with server host and name (currently dummy values provided)

class AutoIndexGenerator
{
	private:
		std::string page; 
		AutoIndexGenerator();
	public:
		AutoIndexGenerator(const char *dirPath);
		~AutoIndexGenerator();
		void genLink(std::string entryName, std::string dirName);
		void genDir(const char *dirName);
		void genPage(const char *dirName);
		void writePage();
		void printPage();
};

#endif