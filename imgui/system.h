#ifndef SYSTEM_H
#define SYSTEM_H

#include <GL/gl3w.h>
#include <memory>
#include "texture.h"

namespace sys
{
    /**
     * @brief Initializes the editor ecosystem
     */
    void init();

    /**
     * @brief Updates the system and does the UI dispatching
     */
    void update();

    /**
     * @brief Creates a new file and opens an editor for it.
     */
    void newFile();

    /**
     * @brief Shows the file dialog and calls openFile(char const *) after
     *        user confirmation.
     */
    void openFile();

    /**
     * @brief Opens the given file into an editor.
     * @param fileName
     */
    void openFile(char const * fileName);


    extern GLuint shader, vao, vbuffer;

    extern std::unique_ptr<Texture> pixel;
}

#endif // SYSTEM_H
