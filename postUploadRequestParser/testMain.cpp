#include "PostUploadRequestParser.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Two arguments required: input file (post request) and output file" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string arg1(argv[1]);
    std::string arg2(argv[2]);

    PostUploadRequestParser purp(arg1, arg2);
    // PostUploadRequestParser purp;
}