# Versa-Tile 3D Model Editor

![Screenshot](https://puu.sh/wJfE6/1dc735ea5f.png)

Versa-Tile is a simple 3D model editor that allows
editing models based on tilesets.

Tiles can be placed on an axis-aligned plane and a
model can be constructed this way.

## Features

- Model Editing
- Undo/Redo
- Vertex Displacement
- Post-Placement Edit (Move/Rotate/Mirror/Displace/…)

## Usage

The editor is mainly controlled with the mouse. On the right hand side is a side bar
that shows the models tile set and allows selection of tiles.

The center of the editor contains a 3D viewport that shows the model and allows editing.

### Navigating in the Viewport
Versa-Tile uses a grid aligned movement for the camera focus. This grid is shown
as white lines and aligns itself to fit a comfortable insertion position.

To rotate the camera, hold the **right mouse button** and drag. Horizontal movement
rotates the camera around the focus point, vertical movement looks up and down.

To move the focus point, press W,A,S,D,Q and E. W moves the camera forward horizontally,
S moves it backwards. A moves to the left and B to the right. E moves the camera upwards
and Q downwards.

For a quick rotation of the camera, all shortcuts listed in the table below.

### Inserting a sprite
To insert a sprite, select the tiles on the right side by clicking and dragging until
the wanted portion is selected. Now a sprite can be inserted in the 3D viewport on
the white grid.

When inserting, the sprite snaps to the grid unless the **Shift** key is pressed, then
a pixel perfect insertion can be done.

When a good position is chosen, a left click will insert the sprite at the previewed
position. Then, another sprite can be inserted. To insert a different sprite, just
select another sprite in the right panel. To end sprite insertion, press **Space**.

### Modifying an existing sprite

![Mode Display](https://puu.sh/wKeUb/7cf3c9f93c.png)

Sprites can be modified in different ways:

#### Translate / Move
Click and drag the sprites center gizmo to move the sprite on its own placement
plane. You can move the sprite up,down,left and right.

#### Rotate Left / Right
Click the corresponding button in the toolbar or menu to rotate the sprite around
its upper left corner.

#### Mirror Horizontal / Vertical
Click the corresponding button in the toolbar or menu to mirror the contents of 
the sprite.

#### Displace Vertex
This feature allows you to displace a vertex to the sprites own plane. Click and
drag a vertex gizmo to displace it along the sprites normal.

This can be used to achieve slopes and skewed sprites.

#### Delete
Click the button on the toolbar or press **Delete** to remove the sprite from the
model.

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
| G            | When AutoGrid is disabled, selects the next grid option      |

## Supported Export Formats
Versa-Tile supports exporting the created 3D models with [Assimp](http://assimp.sourceforge.net/).
Every format that can be exported by Assimp can also be used with Versa-Tile.

On export, the **Y-axis** is **up** and the model is scaled in a way that a single
texture pixel is equivalent to one model unit.

So a model that is 64 pixels wide is also 64 units wide.

## Options

### Behaviour

#### Automatic adjust grid orientation
When this option is active, the grid will align itself with the camera
as soon as it is moved.

#### Horizontal Grid Threshold
This is a factor from `0.0`…`1.0` that scales the **y axis** when determining
the current auto grid.

The lower this is, the later the grid will align into "horizontal" mode,
allowing more camera movement before swapping into horizontal mode.

### Display

#### Ground Display

Determines the way a ground plane is shown. When **None**, no ground plane is
visible, **Grid** is a static horizontal grid, even when the vertical one
is shown and **Solid** is a solid, green surface that is supposed to resemble
a grassy ground.

#### Ground Size
The extends of the ground in tiles. The larger this is, the larger the ground
will be displayed.