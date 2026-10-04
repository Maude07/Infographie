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
| `1` | Select / move / resize |
| `2` `3` `4` `5` | Rectangle, line, point, ellipse |
| `u` | Show / hide the panel |
| `r` | Start / stop recording |
| `i` | Show / hide the scene graph |   
| `Enter` | Add the last chosen color to the palette |
| `Delete` / `Backspace` | Remove the selected object, otherwise the selected palette color |

## Conventions

- Code, identifiers and comments in **English**; text shown in the UI in **French**.
- Architecture: Architecture helper coming soon...
