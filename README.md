Lady-bux
--------

Nonsense ladybug regex builder game.

[![Watch the video](misc/thumb.png)](misc/test_run.webm)
<small>[mp4 version](misc/test_run.mp4)</small>

## Install

### Linux

- compile Raylib dynamically: https://github.com/raysan5/raylib#build-and-installation
- make

### OS-X

- clone into same folder:
  - `git clone git@github.com:itarato/jumper.git`
  - `git clone git@github.com:raysan5/raylib.git`
- `brew install raylib`
- `brew install gcc`
- `export CXX=/usr/local/Cellar/gcc/<INSTALLED_VERSION>/bin/g++`
- `cd jumper`
- `make -f MakefileOSX`
((( TODO: script to build bundle )))

### Runtime options

- `-width INT` window width
- `-height INT` window height
- `fullscreen` full screen (no dash)
- `-map STRING` relative path to map file (to force one)

### Control

Keyboard:

* ← / → : move
* SPACE : jump / double jump
* ALT   : parachute
* CTRL  : dash
* X     : poop (enemy paralyzer)
* BACKPACE : restart level
* ENTER : skip (works on specific screens)

Or compatible gamepad controller (eg XBox).
