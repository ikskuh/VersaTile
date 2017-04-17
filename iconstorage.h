#ifndef ICONSTORAGE_H
#define ICONSTORAGE_H

#include "texture.h"

class IconStorage
{
public:
    const Texture Save, Open, Close, NewMesh, NewTileset;
public:
    IconStorage();
    IconStorage(IconStorage &&) = delete;
    IconStorage(IconStorage const &) = delete;
    ~IconStorage() = default;
};

extern IconStorage * Icons;

#endif // ICONSTORAGE_H
