#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <ctime>

#include "view.h"
#include "window.h"
#include "physicsobject.h"
#include "image.h"
#include "world.h"
#include "texture.h"
#include "spritemap.h"
#include "mousestate.h"
#include "keystate.h"
#include "player.h"
#include "gamestate.h"

#include "tools/packager/packager.h"
#include "logging.h"

extern double FPS, PPS;

int game()
{
    Logger::message("TEST MESSAGE");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    Window mainWindow("GAME", 1000, 1000, 0);

    PackageManager dataPackage("data.bin");

    World world(mainWindow.getRend(), View({1000, 1000}, {0, 0}));

    for(std::string filename : dataPackage.getFileList())
    {
        if(getExtension(filename) == "png")
        {
            std::vector<uint8_t> data = dataPackage.getFile(filename);
            SDL_IOStream* dataBuffer = SDL_IOFromMem(data.data(), data.size());
            SDL_Surface* surf = IMG_Load_IO(dataBuffer, 1);
            Texture::add(SDL_CreateTextureFromSurface(mainWindow.getRend(), surf), filename);
            SDL_DestroySurface(surf);
        }
    }

    world.addImage(new Image({0, 0, 1000, 1000}, "/background.png"));
    world.addPhyObj(new PhysicsObject({0, 960, 1000, 40}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("/Tile.png")));

    world.addPhyObj(new Player({500, 920, 40, 40}, PHYOBJ_COLLIDE, new SpriteMap(mainWindow.getRend(), &dataPackage, "/spritemap.json")));
    world.startPhysics();

    while (!GameState::gameClosing())
    {
        MouseState::update();
        KeyState::update();

        if(KeyState::key(SDL_SCANCODE_Q) == SDL_EVENT_KEY_DOWN)
            GameState::closeGame();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    GameState::closeGame();
                    break;
                }
                default:
                {

                }
            }
        }

        if(MouseState::scrollDelta() != 0)
        {
            world.getView().setZoom(std::max(std::min(world.getView().getZoom() + 0.05 * (float)MouseState::scrollDelta(), 2.0), 0.1));
        }
        if(MouseState::buttonDown(SDL_BUTTON_RIGHT))
        {
            world.getView().moveDelta(MouseState::mouseDelta());
        }
        mainWindow.render(world);
        world.runPhysics();
        SDL_Delay(1);
        std::cout << "FPS: " << FPS << "\tPPS:" << PPS << "\r";
    }

    world.stopPhysics();
    SDL_Quit();

    return 0;
}