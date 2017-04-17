#ifndef SYSTEM_H
#define SYSTEM_H

namespace sys
{
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
}

#endif // SYSTEM_H
