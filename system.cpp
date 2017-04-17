#include <nfd.h>
#include <iostream>
#include "system.h"
#include "mesheditor.h"
#include <libgen.h>
#include <string.h>
#include <vector>

namespace sys
{
    static nfdchar_t * workingDirectory = nullptr;
    static std::vector<MeshEditor> editors;

    void newFile()
    {

    }

    void openFile()
    {
        nfdchar_t *path;
        auto result = NFD_OpenDialog(nullptr, workingDirectory, &path);
        switch(result)
        {
            case NFD_OKAY:
                if(workingDirectory != nullptr) {
                    free(workingDirectory);
                }
                workingDirectory = strdup(path);
                dirname(workingDirectory);

                openFile(path);

                free(path);
                break;
            case NFD_CANCEL:
                break;
            case NFD_ERROR:
                std::cerr << "[ERR] " << NFD_GetError() << std::endl;
                break;
        }
    }

    void openFile(char const * fileName)
    {
        std::cout << "Open file '" << fileName << "'" << std::endl;
    }
}
