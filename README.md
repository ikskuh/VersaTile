# VersaTile 3D Model Editor

![Screenshot](https://puu.sh/wZtBW/e4b61754c2.png)

*VersaTile* is a simple 3D model editor that allows
editing models based on tilesets.

Tiles can be placed on an axis-aligned plane and a
model can be constructed this way.

## Features

- Model editing
- Undo/redo
- Vertex displacement
- Post-placement edit (move/rotate/mirror/displace/…)
- Export via Assimp

## Usage

The editor is mainly controlled with the mouse. On the right hand side is a side bar
that shows the models sprite sheet and allows the selection of sprites.

The center of the editor contains a 3D viewport that shows the model and allows editing.

### Creating a model

When creating a new model, the following dialog appears:

![Create new model dialog](https://puu.sh/wZtEf/189a1e35fa.png)

It provides several options that must be set up correctly to import a sprite sheet:

- **File Name**
  This option is required and defines the file that should be used as the sprite sheet.
- **Sprite Size**
  This option defines the size of the smallest tile that can be selected in the sprite
  sheet view. This also defines the grid size.
- **Spritesheet Margin**
  If the sprite sheet image has a border around the contained sprites, set this value to
  the number of pixels this border is wide.
- **Sprite Padding**
  If there is a padding between the single sprites in the sheet, set this value to the
  number of pixels this padding is wide.
- **Preview**
  Here you can see a preview of the imported sprites. The magenta lines will disappear in
  the editor, but are displayed here for you to check if your setup of the sprite sheet
  is correct.

### Navigating in the Viewport

![Generic Controls](https://puu.sh/wZuey/352a19e855.png)

*VersaTile* uses a grid aligned movement for the camera focus. This grid is shown
as white lines and aligns itself to fit a comfortable insertion position.

To rotate the camera, hold the **right mouse button** and drag. Horizontal movement
rotates the camera around the focus point, vertical movement looks up and down.

To move the focus point, press W,A,S,D,Q and E. W moves the camera forward horizontally,
S moves it backwards. A moves to the left and B to the right. E moves the camera upwards
and Q downwards.

For a quick rotation of the camera, all shortcuts listed in the table below.

If the option `Automatic adjust grid orientation` is set, the displayed grid will align
with the camera so always the most convenient grid is active. If this option is disabled,
the active grid can be changed by pressing **G** or the **Toggle Grid Plane** button.

### Inserting a tile
To insert a tile, select the sprite on the right side by clicking and dragging until
the wanted portion is selected. Now a tile can be inserted in the 3D viewport on
the white grid.

When inserting, the tile snaps to the grid unless the **Shift** key is pressed, then
a pixel perfect insertion can be done.

When a good position is chosen, a left click will insert the tile at the previewed
position. Then, another tile can be inserted. To insert a different tile, just
select another sprite in the right panel. To end tile insertion, press **Space** or
use the **Selection mode** button in the toolbar.

### Modifying an existing tile

![Modify Display](https://puu.sh/wZudI/c796116a40.png)

Tiles can be modified in different ways:

#### Translate / Move
Click and drag the tiles center gizmo to move the tile on its own placement
plane. You can move the tile up,down,left and right.

#### Rotate Left / Right
Click the corresponding button in the toolbar or menu to rotate the tile around
its upper left corner.

#### Flip Horizontal / Vertical
Click the corresponding button in the toolbar or menu to mirror the contents of 
the tile.

#### Displace Vertex
This feature allows you to displace a vertex to the tiles own plane. Click and
drag a vertex gizmo to displace it along the tiles normal.

This can be used to achieve slopes and skewed tiles.

#### Delete
Click the button on the toolbar or press **Delete** to remove the tile from the
model.

## Controls

| Key          | Function                                                     |
|--------------|--------------------------------------------------------------|
| W,A,S,D      | Move camera focus horizontal                                 |
| Q,E          | Move camera focus up/down                                    |
| Space        | Cancel/Stop tile insertion                                   |
| H            | Flip horizontal                                              |
| V            | Flip vertical                                                |
| R,Ctrl+R     | Rotate tile                                                  |
| Shift        | Enable fine grid snapping (pixel perfect)                    |
| Num-2        | Rotate camera down 90°                                       |
| Num-8        | Rotate camera up 90°                                         |
| Num-4        | Rotate camera left 90°                                       |
| Num-6        | Rotate camera right 90°                                      |
| Num-Plus     | Zoom camera in                                               |
| Num-Minus    | Zoom camera out                                              |
| Right Mouse  | Drag to rotate camera                                        |
| Left Mouse   | Click to select, modify or insert tile                       |
| Delete       | Remove the selected tile                                     |
| Ctrl+Z       | Undo the last modification                                   |
| Ctrl+Y       | Redo the last undone modification                            |
| Ctrl+H       | Rotate camera into **Home** position                         |
| Ctrl+T       | Rotate camera into **Top** position                          |
| Ctrl+F       | Rotate camera into **Front** position                        |
| Ctrl+G       | Rotate camera into **Side** position                         |
| Ctrl+Shift+H | Set camera home position                                     |
| G            | When AutoGrid is disabled, selects the next grid option      |
| F            | Moves the camera center to the currently selected tile.      |

## Supported Export Formats
*VersaTile* supports exporting the created 3D models with [Assimp](http://assimp.sourceforge.net/).
Every format that can be exported by Assimp can also be used with *VersaTile*.

On export, the **Y-axis** is **up** and the model is scaled in a way that a single
texture pixel is equivalent to one model unit.

So a model that is 64 pixels wide is also 64 units wide.

## Preferences

In this dialog, you can set up your preferences on how *VeraTile* should behave.

### Behaviour

![Preferences dialog: Behaviour](https://puu.sh/wZtOm/c7513f9e86.png)

#### Automatic adjust grid orientation
When this option is active, the grid will align itself with the camera
as soon as it is moved.

#### Horizontal Grid Threshold
This is a factor from `0.0`…`1.0` that scales the **y axis** when determining
the current auto grid.

The lower this is, the later the grid will align into "horizontal" mode,
allowing more camera movement before swapping into horizontal mode.

### Display

![Preferences dialog: Display](https://puu.sh/wZtOY/36fa3cb9e2.png)

#### Ground Display

Determines the way a ground plane is shown. When **None**, no ground plane is
visible, **Grid** is a static horizontal grid, even when the vertical one
is shown and **Solid** is a solid, green surface that is supposed to resemble
a grassy ground.

#### Ground Size
The extends of the ground in tiles. The larger this is, the larger the ground
will be displayed.

#### Show Coordinate Axis
If this option is enabled, the coordinate axis are shown by three short lines in the
colors red (x-axis), green (y-axis) and blue (z-axis). The intersection of the three
lines is the current camera focus point.

## Credits

Thanks for each [Crocotile3D](http://www.crocotile3d.com/) and [Sprytile](https://github.com/ChemiKhazi/Sprytile)
for inspiring me to create *VersaTile*!

Also thanks to [kenney.nl](http://kenney.nl/assets/roguelike-caves-dungeons) for delivering our default tile
set! It suits this editor very much.

## More Screenshots

![](https://puu.sh/wYFlX/6b0fa22731.png)

![](https://puu.sh/wXGND/2a17f8ef45.png)

