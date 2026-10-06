# TP Infographie — IFT3100

2D drawing application built with openFrameworks 0.12.1 and ofxGui.

## Building

- **macOS**: put the project in `of_v0.12.1_osx_release/apps/myApps/`, then run `make && make RunRelease`.
- **Windows**: open `TP_Infographie.sln` in Visual Studio 2022.
- After adding, moving or deleting files in `src/`, update `TP_Infographie.vcxproj`
  (projectGenerator, or by hand).

## Keyboard shortcuts

| Key | Action |
|---|---|
| `1` | Select / move / resize / rotate |
| `2` `3` `4` `5` | Rectangle, line, point, ellipse |
| `u` | Show / hide the panel |
| `r` | Start / stop recording |
| `i` | Show / hide the scene graph |   
| `Enter` | Add the last chosen color to the palette |
| `Delete` / `Backspace` | Remove the selected object, otherwise the selected palette color |
| `Shift` + drag the resize handle | Keep the width-to-height ratio |
| `Shift` + drag the rotate handle | Rotate in 15 degree steps |

## Transformations

In selection mode (`1`), click an object to select it, then:

- **Move**: drag the object.
- **Resize**: drag the square handle at the bottom-right corner. The opposite corner stays in place, even when the object is rotated.
- **Rotate**: drag the round handle above the object. It turns around its center.

Points can only be moved

The **transformation** group in the panel shows the selected object's position, rotation and size, and you can type exact values there.

## Conventions

- Code, identifiers and comments in **English**; text shown in the UI in **French**.
- Architecture: Architecture helper coming soon...
