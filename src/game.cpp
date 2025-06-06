#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3_image/SDL_image.h>

#include <memory>
#include <vector>

#include "gamestate.h"
#include "graphics.h"
#include "image.h"
#include "keystate.h"
#include "mousestate.h"
#include "physicsobject.h"
#include "player.h"
#include "shader.h"
#include "spritemap.h"
#include "texture.h"
#include "view.h"
#include "window.h"
#include "world.h"

#include "logging.h"
#include "tools/packager/packager.h"

using namespace GameEng;

// Simple world update function for keyboard/mouse updates
void worldFunc(double deltaTime, World& world)
{
    (void)deltaTime;

    MouseState::update();
    KeyState::update();

    if(KeyState::keyPressed(SDL_SCANCODE_Q))
        GameState::closeGame();

    if(KeyState::keyPressed(SDL_SCANCODE_P))
        GameState::togglepause();

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_EVENT_QUIT: {
            GameState::closeGame();
            break;
        }
        default: {
        }
        }
    }

    if(MouseState::scrollDelta() != 0)
    {
        world.getView().setZoom(
            std::max(std::min(world.getView().getZoom() - 0.05 * (float)MouseState::scrollDelta(), 2.0), 0.1));
    }

    if(MouseState::buttonDown(SDL_BUTTON_RIGHT))
    {
        world.getView().moveDelta(MouseState::mouseDelta());
    }
}

int game()
{
    Window mainWindow("GAME", 1000, 1000, 0, GPUDevice::getGPU());

    Packager::PackageManager dataPackage("data.bin");

    // Create the default shaders
    std::vector<uint8_t> data = dataPackage.getFile("/shader.vert.hlsl");
    std::vector<uint8_t> buildShader = Shader::buildShader("shader.vert.hlsl", data);
    Shader::add(Shader::LoadShaderFromArray(mainWindow.getGPU(), "shader.vert.spv", buildShader, 0, 2, 1, 0),
                "default.vert");
    if(!Shader::exists("default.vert"))
    {
        Logger::error("Failed to create default vertex shader!");
        return -1;
    }

    data = dataPackage.getFile("/shader.frag.hlsl");
    buildShader = Shader::buildShader("shader.frag.hlsl", data);
    Shader::add(Shader::LoadShaderFromArray(mainWindow.getGPU(), "shader.frag.spv", buildShader, 1, 0, 0, 0),
                "default.frag");
    if(!Shader::exists("default.frag"))
    {
        Logger::error("Failed to create default frag shader!");
        return -1;
    }

    // Create default Sampler
    SDL_GPUSamplerCreateInfo gpuSampleInfo;
    SDL_zero(gpuSampleInfo);
    gpuSampleInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    gpuSampleInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    gpuSampleInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    gpuSampleInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    gpuSampleInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    gpuSampleInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    gpuSampleInfo.enable_anisotropy = true;
    gpuSampleInfo.max_anisotropy = 4;

    Sampler::add(Sampler::createSampler(mainWindow, gpuSampleInfo), "default");
    if(!Sampler::exists("default"))
    {
        Logger::error("Failed to create default sampler!");
        return -1;
    }

    // Create default pipeline
    Pipeline::add(Pipeline::createPipeline(mainWindow, "default.vert", "default.frag"), "default");
    if(!Pipeline::exists("default"))
    {
        Logger::error("Failed to create default pipeline!");
        return -1;
    }

    for(std::string filename : dataPackage.getFileList())
    {
        if(getExtension(filename) == "png")
        {
            data = dataPackage.getFile(filename);
            SDL_IOStream* dataBuffer = SDL_IOFromMem(data.data(), data.size());
            SDL_Surface* surf = IMG_Load_IO(dataBuffer, 1);
            Texture::add(createTexture(mainWindow.getGPU(), surf, filename), filename);

            Logger::message("uploaded " + filename);
            SDL_DestroySurface(surf);
        }
    }

    World world(mainWindow.getGPU(), View({1000, 1000}, {500, 500}));
    world.registerUpdate(worldFunc);
    world.addObj(new Image({0, 0, 1000, 1000}, "/background.png"));
    world.addObj(new PhysicsObject({0, 950, 1000, 50}, PhyObjFlag::Static | PhyObjFlag::Collide,
                                   std::make_shared<Texture>("/Tile.png")));
    world.addObj(new PhysicsObject({450, 650, 500, 50}, PhyObjFlag::Static | PhyObjFlag::Collide,
                                   std::make_shared<Texture>("/Tile.png")));
    world.addObj(new Player({500, 920, 40, 40}, PhyObjFlag::Collide,
                            std::make_shared<SpriteMap>(&dataPackage, "/spritemap.json")));
    world.startPhysics();

    Timer stats(1.0f);
    int frames = 0;
    while(!GameState::gameClosing())
    {

        if(mainWindow.render(world))
        {
            frames++;
        }

        world.runPhysics();

        if(stats.trigger())
        {
            double time = stats.getElapsed();
            Logger::message("FPS: ", frames / time * 1000.0f);
            frames = 0;
            stats.update();
        }

        SDL_Delay(0);
    }

    world.stopPhysics();
    return 0;
}
