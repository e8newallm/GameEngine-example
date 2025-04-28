# GameEngine-Example

An example project for the SDL3 based game engine found [here](https://github.com/e8newallm/GameEngine).

## Dependencies
 - [SDL3](https://github.com/libsdl-org/SDL)
 - [SDL3_image](https://github.com/libsdl-org/SDL_image)
 - [Catch2](https://github.com/catchorg/Catch2)
 - [Rapidjson](https://github.com/Tencent/rapidjson)

## Build

Before building the example project, make sure all of the dependencies are installed.

1. Clone the repository
```sh
git clone https://github.com/e8newallm/GameEngine-example.git
```
2. Pull all submodules used by the project (Including the GameEngine itself)
```sh
git submodule update --init --recursive
```
3. Build the example project (Add the -r flag to run the project after build)
```sh
./build.sh
./build.sh -r
```