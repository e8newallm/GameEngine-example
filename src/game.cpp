#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_thread.h>

#include <ctime>

#include "physicsobject.h"
#include "image.h"
#include "context.h"
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
    SDL_Window* win = SDL_CreateWindow("GAME", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       1000, 1000, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    KeyState& keyState = KeyState::get();
    MouseState& mouseState = MouseState::get();

    PackageManager dataPackage("data.bin");
    View viewport( {1000, 1000}, {0, 0});
    viewport.setZoom(1.0);

    Context state(rend, &viewport);

    PhysicsContext* phyContext = state.getPhysicsContext();

    for(std::string filename : dataPackage.getFileList())
    {
        if(getExtension(filename) == "png")
        {
            std::vector<uint8_t> data = dataPackage.getFile(filename);
            SDL_RWops* dataBuffer = SDL_RWFromMem(data.data(), data.size());
            SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, IMG_Load_RW(dataBuffer, 1));
            Texture::add(tex, filename);
        }
    }

    state.addImage(new Image({0, 0, 1000, 1000}, new Texture("/background.png"), UINT8_MAX));
    phyContext->addPhyObj(new PhysicsObject({0, 960, 1000, 40}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("/Tile.png")));

    phyContext->addPhyObj(new Player({500, 920, 40, 40}, PHYOBJ_COLLIDE, new SpriteMap(rend, &dataPackage, "/spritemap.json")));
    state.startPhysics();

    while (!gameClosing)
    {
        mouseState.reset();

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
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    keyState.update(event.key.keysym.scancode, event.type);
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    mouseState.updateButton(event.button);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    mouseState.updateMove(event.motion);
                    break;
                }
                case SDL_MOUSEWHEEL:
                {
                    mouseState.updateWheel(event.wheel);
                    break;
                }
                default:
                {
                       
                }
            }
        }

        if(mouseState.scrollDelta() != 0)
        {
            viewport.setZoom(std::max(std::min(viewport.getZoom() + 0.05 * (float)mouseState.scrollDelta(), 2.0), 0.1));
        }
        if(mouseState.buttonDown(SDL_BUTTON_RIGHT))
        {
            SDL_Point newPosition = viewport.getPosition();
            SDL_Point delta = mouseState.mouseDelta();
            newPosition.y -= delta.y / viewport.getZoom();
            newPosition.x += delta.x / viewport.getZoom();
            viewport.setPosition(newPosition);
        }
        
        SDL_RenderClear(rend);
        state.draw();
        SDL_RenderPresent(rend);
        //SDL_Delay(10);
    }
    state.stopPhysics();
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
 
    return 0;
}