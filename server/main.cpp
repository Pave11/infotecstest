#include "server.h"
#include <cstdlib>

int main(int argc, char* argv[])
{
    int port = 4040;
    std::string filePath = "/tmp/file_src";

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-p" && i + 1 < argc)
        {
            port = std::stoi(argv[++i]);
        }
        else if (arg == "-file" && i + 1 < argc)
        {
            filePath = argv[++i];
        }
    }

    Server server(port, filePath);
    server.run();

    return 0;
}
