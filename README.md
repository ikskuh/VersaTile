# Versa-Tile 3D Model Editor

![Screenshot](https://puu.sh/wJfE6/1dc735ea5f.png)

Versa-Tile is a simple 3D model editor that allows
editing models based on tilesets.

Tiles can be placed on an axis-aligned plane and a
model can be constructed this way.

## Features

- Model Editing
- Undo
- Vertex Displacement
- Post-Placement Edit (Move/Rotate/Flip/…)

## Usage

## Controls

| Key          | Function                                                     |
|--------------|--------------------------------------------------------------|
| W,A,S,D      | Move camera focus horizontal                                 |
| Q,E          | Move camera focus up/down                                    |
| Space        | Cancel/Stop sprite insertion                                 |
| H            | Flip horizontal                                              |
| V            | Flip vertical                                                |
| R,Ctrl+R     | Rotate sprite                                                |
| Shift        | Enable fine grid snapping (pixel perfect)                    |
| Num-2        | Rotate camera down 90°                                       |
| Num-8        | Rotate camera up 90°                                         |
| Num-4        | Rotate camera left 90°                                       |
| Num-6        | Rotate camera right 90°                                      |
| Num-Plus     | Zoom camera in                                               |
| Num-Minus    | Zoom camera out                                              |
| Right Mouse  | Drag to rotate camera                                        |
| Left Mouse   | Click to select, modify or insert sprite                     |
| Delete       | Remove the selected sprite                                   |
| Ctrl+Z       | Undo the last modification                                   |
| Ctrl+H       | Rotate camera into **Home** position                         |
| Ctrl+T       | Rotate camera into **Top** position                          |
| Ctrl+F       | Rotate camera into **Front** position                        |
| Ctrl+G       | Rotate camera into **Side** position                         |
| Ctrl+Shift+H | Set camera home position                                     |

## Supported Export Formats
> TBD

All files types that can be exported with the [Assimp](http://assimp.sourceforge.net/) library.
