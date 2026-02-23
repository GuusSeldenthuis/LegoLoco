# openLegoLoco

[![Build & Release](https://github.com/GuusSeldenthuis/LegoLoco/actions/workflows/build.yml/badge.svg)](https://github.com/GuusSeldenthuis/LegoLoco/actions/workflows/build.yml)

> **Early Development** — This is a one-man hobby project and is far from complete. Expect missing features, rough edges, and frequent breaking changes. Contributions and feedback are welcome, but please keep expectations in check.

![Screenshot](images/Screenshot_02.png)

An open-source recreation of [LEGO Loco (1998)](https://en.wikipedia.org/wiki/Lego_Loco), the LEGO train-building sandbox game, built with C++ and raylib.

## Roadmap

The project is structured around 8 main milestones toward a first playable version.

| Milestone | Status |
|---|---|
| 1. Core Engine — grid, tiles, camera, world save/load | Done |
| 2. User Interface — main menu, toy box, game states | In progress |
| 3. Buildings & Objects — placement, houses, stations, scenery | Partially done |
| 4. Track System — switches, pathfinding, junctions | Partially done |
| 5. Trains — movement, carriages, speed control | Not started |
| 6. Minifigures — spawning, walking AI, train boarding | Not started |
| 7. Simulation Loop — tick rate, pause/speed controls | Not started |
| 8. Polish — audio, particles, undo/redo, settings | Not started |

Further out: multiplayer (LAN), postcard system, seasons/weather, train customization, and easter eggs from the original game.

See [ToDo.md](ToDo.md) for the full detailed task list.

## Dependencies (Linux)

```bash
sudo apt install libx11-dev libxrandr-dev libxi-dev libxinerama-dev libxcursor-dev
sudo apt install libgl1-mesa-dev libglu1-mesa-dev
```

## Clone

```bash
git clone --recursive https://github.com/GuusSeldenthuis/openLegoLoco.git
```

Or if already cloned without submodules:
```bash
git submodule update --init --recursive
```

## Build & Run

```bash
make          # Build raylib (first time) and the project
make run      # Build and run
make clean    # Remove only the game binary
make clean-all  # Remove binary and clean raylib build
```

### CMake (alternative)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./bin/lego_loco
```

## Acknowledgments

Thanks to [shinyquagsire23](https://github.com/shinyquagsire23) for creating [rf-extract.py](https://gist.github.com/neofelis2X/fd244e45eafef0c90a1eafed9041abd3) and [neofelis2X](https://github.com/neofelis2X) for making it compatible with Python 3.

## Disclaimer

This is a fan-made, non-commercial project created for educational and entertainment purposes only. "LEGO" and "LEGO Loco" are trademarks of the LEGO Group, which does not sponsor, authorize, or endorse this project. Any original game assets used are the property of their respective owners. This project is not affiliated with or endorsed by the LEGO Group in any way.
