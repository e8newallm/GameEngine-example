#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_thread.h>

#include <SDL_events.h>
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

#include "tools/packager/packager.h"
#include "logging.h"

extern bool gameClosing;

int game()
{
    Logger::message("TEST MESSAGE");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
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
            SDL_RWops* dataBuffer = SDL_RWFromMem(data.data(), data.size());
            SDL_Surface* surf = IMG_Load_RW(dataBuffer, 1);
            Texture::add(SDL_CreateTextureFromSurface(mainWindow.getRend(), surf), filename);
            SDL_FreeSurface(surf);
        }
    }

    world.addImage(new Image({0, 0, 1000, 1000}, "/background.png"));
    world.addPhyObj(new PhysicsObject({0, 960, 1000, 40}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("/Tile.png")));

    world.addPhyObj(new Player({500, 920, 40, 40}, PHYOBJ_COLLIDE, new SpriteMap(mainWindow.getRend(), &dataPackage, "/spritemap.json")));
    world.startPhysics();

    while (!gameClosing)
    {
        MouseState::update();
        KeyState::update();

        if(KeyState::key(SDL_SCANCODE_Q) == SDL_KEYDOWN)
            gameClosing = true;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    gameClosing = true;
                    break;
                }
                default:
                {

                }
            }
        }

        if(MouseState::scrollDelta() != 0)
        {
            world.getViewpoint().setZoom(std::max(std::min(world.getViewpoint().getZoom() + 0.05 * (float)MouseState::scrollDelta(), 2.0), 0.1));
        }
        if(MouseState::buttonDown(SDL_BUTTON_RIGHT))
        {
            SDL_Point newPosition = world.getViewpoint().getPosition();
            SDL_Point delta = MouseState::mouseDelta();
            newPosition.y -= delta.y / world.getViewpoint().getZoom();
            newPosition.x += delta.x / world.getViewpoint().getZoom();
            world.getViewpoint().setPosition(newPosition);
        }
        mainWindow.render(world);
    }

    world.stopPhysics();
    SDL_Quit();

    return 0;
}