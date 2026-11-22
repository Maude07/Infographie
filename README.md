# TP Infographie — IFT3100

2D drawing application built with openFrameworks 0.12.1 and ofxGui.

## Building

- **macOS**: put the project in `of_v0.12.1_osx_release/apps/myApps/`, then run `make && make RunRelease`.
- **Windows**: open `TP_Infographie.sln` in Visual Studio 2022.
- After adding, moving or deleting files in `src/`, update `TP_Infographie.vcxproj`
  (projectGenerator, or by hand).


## Dependencies

- **ofxGui** and **ofxAssimpModelLoader** must be present in your openFrameworks `addons/` folder (see `addons.make`).
- Bezier path fitting uses a vendored copy of ofxPathFitter in `src/third_party/ofxPathFitter/` (no separate addon install).

## Keyboard shortcuts

| Key | Action |
|---|---|
| `1` | Select / move / resize |
| `2` `3` `4` `5` `6` `7` `8` `b` `k` | Rectangle, line, point, ellipse, star, waves, funkyCircles, bezier, Catmull-Rom spline |
| `9` | 3D primitive; press again to cycle cube, sphere, cylinder, cone |
| `n` | Animated sprite; press again to cycle animations (ball, coin, generated sheets) |
| `u` | Show / hide the panel |
| `r` | Start / stop recording |
| `i` | Show / hide the scene graph | 
| `Left Arrow` / `Right Arrow` | Rotate the selected 3D objects around their vertical axis |  
| `Enter` | Finish the Catmull-Rom spline being drawn, otherwise add the last chosen color to the palette |
| `Delete` / `Backspace` | Remove the last Catmull-Rom control point while drawing, otherwise remove the selected object, otherwise the selected palette color |
| `Shift` + drag the resize handle | Keep the width-to-height ratio |
| `Shift` + drag the rotate handle | Rotate in 15 degree steps |

## Transformations

In selection mode (`1`), click an object to select it, then:

- **Move**: drag the object.
- **Resize**: drag the square handle at the bottom-right corner. The opposite corner stays in place, even when the object is rotated.
- **Rotate**: drag the round handle above the object. It turns around its center.

Points can only be moved

The **inspector** on the right side of the window (like in Unity) shows the selected object's name, position, rotation and size, and you can type exact values there. It is hidden with the rest of the interface (`u`).

## Parametric curve (Catmull-Rom)

Press `k`, then left-click to place control points. The curve passes through every point
and a live preview follows the mouse.

- At least **5 control points** are required; `Enter` finishes the curve.
- `Backspace` removes the last point; switching tools cancels the curve.
- The grey control polygon stays visible on the finished curve.
- Once created, the curve can be moved, resized and rotated like any other shape (`1`), and undone with `Ctrl+Z`.

Implementation: uniform Catmull-Rom, evaluated in `SceneShapeCatmullRom::evaluate()`
(`src/vector_shapes/sceneShapeCatmullRom.cpp`), 20 samples per segment.

## Conventions

- Code, identifiers and comments in **English**; text shown in the UI in **French**.
- Architecture: Architecture helper coming soon...
