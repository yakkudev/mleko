# mleko
a game written in c.
work in progress

## building
```
make all
```

supported systems: linux

## running
```
cd bin 
./launcher
```
The launcher will listen for changes at `./touch.point` and reload `./libgame.so`.
rebuilding the game with `make game` will automatically update the touch point


## IDE (nvim) setup
for clangd to work, use `bear` to generate `compile_commands.json` (run `make clean` before this!)
```
bear -- make all
```

## LICENSE
All Rights Reserved. This software is for viewing and educational purposes only. 
You may not copy, modify, distribute, or use the code in any project, commercial or non-commercial.

## Third-Party Libraries
The project uses the following libraries:
- [cglm], licensed under the MIT License
- [glad], licensed under the (WTFPL OR CC0-1.0) AND Apache-2.0 licenses

The full source code and license information for these libraries can be found in their respective directories.

