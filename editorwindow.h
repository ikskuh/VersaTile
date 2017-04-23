#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <string>

class EditorWindow
{
public:
    virtual ~EditorWindow() = default;

    virtual void update(const std::string & name) = 0;
};

#endif // EDITORWINDOW_H
