#include "AutoIndexGenerator.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Program requires single arg (path to generate index.html for)" << std::endl;
		exit(EXIT_FAILURE);
	}
    const char *dirPath = argv[1];
    AutoIndexGenerator aig(dirPath);
    aig.printPage();
}