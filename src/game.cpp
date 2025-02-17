#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
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
#include "shader.h"
#include "graphics.h"

#include "tools/packager/packager.h"
#include "logging.h"

extern double FPS, PPS;

void worldFunc(double deltaTime, World& world)
{
	(void)deltaTime;

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
		world.getView().setZoom(std::max(std::min(world.getView().getZoom() - 0.05 * (float)MouseState::scrollDelta(), 2.0), 0.1));
	}

	if(MouseState::buttonDown(SDL_BUTTON_RIGHT))
	{
		world.getView().moveDelta(MouseState::mouseDelta());
	}
}

int game()
{
    Window mainWindow("GAME", 1000, 1000, 0);

    PackageManager dataPackage("data.bin");

	// Create the default shaders
	Shader::add(Shader::LoadShaderFromFile(mainWindow.getGPU(), "shader.vert.spv", 0, 2, 1, 0), "default.vert");
	if (Shader::get("default.vert") == nullptr)
	{
		Logger::error("Failed to create default vertex shader!");
		return -1;
	}

	Shader::add(Shader::LoadShaderFromFile(mainWindow.getGPU(), "shader.frag.spv",  1, 0, 0, 0), "default.frag");
	if (Shader::get("default.frag") == nullptr)
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
	if (Sampler::get("default") == nullptr)
	{
		Logger::error("Failed to create default sampler!");
		return -1;
	}

	// Create default pipeline
	Pipeline::add(Pipeline::createPipeline(mainWindow, "default.vert", "default.frag"), "default");
	if (Pipeline::get("default") == nullptr)
	{
		Logger::error("Failed to create default pipeline!");
		return -1;
	}

    for(std::string filename : dataPackage.getFileList())
    {
        if(getExtension(filename) == "png")
        {
            std::vector<uint8_t> data = dataPackage.getFile(filename);
            SDL_IOStream* dataBuffer = SDL_IOFromMem(data.data(), data.size());
            SDL_Surface* surf = IMG_Load_IO(dataBuffer, 1);
			Logger::message("uploading " + filename);
            SDL_GPUTexture* tex = uploadTexture(mainWindow.getGPU(), surf, filename);

            Texture::add(new GPUTexture{tex, surf->w, surf->h}, filename);
			Logger::message("uploaded " + filename);
            SDL_DestroySurface(surf);
        }
    }

    World world(mainWindow.getGPU(), View({1000, 1000}, {500, 500}));
	world.registerUpdate(worldFunc);
    world.addObj(new Image({0, 0, 1000, 1000}, "/background.png"));
    world.addObj(new PhysicsObject({0, 950, 1000, 50}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("/Tile.png")));
    world.addObj(new Player({500, 920, 40, 40}, PHYOBJ_COLLIDE, new SpriteMap(&dataPackage, "/spritemap.json")));
    world.startPhysics();

    while (!GameState::gameClosing())
    {
		mainWindow.render(world);
        world.runPhysics();

		SDL_Delay(0);
		std::cout << "FPS: " << FPS << "\t PPS:" << PPS << "\r\n";
	}

	world.stopPhysics();
    return 0;
}