#include "PostUploadRequestParser.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "One arguments required: input file (post request)" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string arg1(argv[1]);
	PostUploadRequestParser purp(arg1);
	// testing if getStruct works
	// std::cout << purp.getStruct().httpVersion << std::endl;
	// std::cout << purp.getStruct().parts[0].contentDisposition.type << std::endl;
	// std::cout << purp.getStruct().parts[0].contentDisposition.name << std::endl;
	// std::cout << purp.getStruct().parts[0].contentDisposition.filename << std::endl;
}