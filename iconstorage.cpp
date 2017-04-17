#include "iconstorage.h"

IconStorage * Icons = nullptr;

IconStorage::IconStorage() :
    Save("data/save.png"),
    Open("data/open.png"),
    Close("data/close.png"),
    NewMesh("data/new-mesh.png"),
    NewTileset("data/new-tileset.png")
{

}
