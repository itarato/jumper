Lady-bux
--------

Nonsense ladybug regex builder game. The ladybug needs to escape the enemy swarmed land though regex locked gates which can be opened by carefully sequence-collected pattern combos.

[![Watch the video](misc/thumb.png)](misc/test_run.webm)
<small>[mp4 version](misc/test_run.mp4)</small>

## Install

### Linux

- compile Raylib dynamically: https://github.com/raysan5/raylib#build-and-installation
- `make`

### OS-X

- `brew install raylib`
- `git clone git@github.com:itarato/jumper.git`
- `cd jumper`
- `make -f MakefileOSX bundle`

### Windows (not tested with latest version)

- install MinGW: https://www.mingw-w64.org/downloads/#mingw-builds
- clone into same folder:
  - `git clone git@github.com:itarato/jumper.git`
  - `git clone git@github.com:raysan5/raylib.git`
- build raylib: https://github.com/raysan5/raylib/wiki/Working-on-Windows
- `mingw32-make.exe -f .\MakefileWIN`

### Runtime options

Configuration can be either added as command line arguments or a configuration file - or mixed (last one overrides).

```bash
$> ./jumper -conf default.conf -width 1280
```

For configuration options see [example.conf](./example.conf).

### Control

Keyboard:

* ← / → : move
* SPACE : jump / double jump
* ALT   : parachute
* CTRL  : dash
* X     : poop (enemy paralyzer)
* BACKPACE : restart level
* ENTER : skip (works on specific screens)
* P     : pause

Or compatible gamepad controller (eg XBox).

---

Wild west theme design credit to [Sandor Horvath](https://www.behance.net/lexdraw/appreciated)
