#ifndef TILESETVIEWER_H
#define TILESETVIEWER_H

#include "texture.h"
#include "editorwindow.h"
#include <memory>
#include <string>

class TilesetViewer : public EditorWindow
{
private:
    std::shared_ptr<Texture> mTileset;
    int mTileSize;
public:
    TilesetViewer(std::shared_ptr<Texture> const & tileset);

    void update(std::string const & name) override;

    std::shared_ptr<Texture> const & tileset() const { return this->mTileset; }
};

#endif // TILESETVIEWER_H
