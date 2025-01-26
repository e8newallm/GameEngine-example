#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
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


////////////////////////////////
SDL_GPUGraphicsPipeline* Pipeline;
SDL_GPUSampler* sample;
SDL_GPUBuffer* vertexBuffer;
SDL_GPUBuffer* indexBuffer;
SDL_GPUShader* vertexShader;
SDL_GPUShader* fragmentShader;

struct vertDat
{
    float vertices[4][4];
    Uint16 indexData[6];
};

struct vertDat verticesData = {
	{
        {-0.5f, 0.5f, 0.0f, 0.0f},
        {0.5f, 0.5f, 1.0f, 0.0f},
        {0.5f, -0.5f, 1.0f, 1.0f},
        {-0.5f, -0.5f, 0.0f, 1.0f}
    },
	{
        0, 1, 2, 0, 2, 3
    }
};

SDL_GPUShader* LoadShader(
	SDL_GPUDevice* device,
	const char* shaderFilename,
	Uint32 samplerCount,
	Uint32 uniformBufferCount,
	Uint32 storageBufferCount,
	Uint32 storageTextureCount
) {
	// Auto-detect the shader stage from the file name for convenience
	SDL_GPUShaderStage stage;
	if (SDL_strstr(shaderFilename, ".vert"))
	{
		stage = SDL_GPU_SHADERSTAGE_VERTEX;
	}
	else if (SDL_strstr(shaderFilename, ".frag"))
	{
		stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	}
	else
	{
		SDL_Log("Invalid shader stage!");
		return NULL;
	}

	char fullPath[256];
	SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
	const char *entrypoint;

	if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s.spv", ".", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s.msl", ".", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_MSL;
		entrypoint = "main0";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s.dxil", ".", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_DXIL;
		entrypoint = "main";
	} else {
		SDL_Log("%s", "Unrecognized backend shader format!");
		return NULL;
	}

	size_t codeSize;
	void* code = SDL_LoadFile(fullPath, &codeSize);
	if (code == NULL)
	{
		SDL_Log("Failed to load shader from disk! %s", fullPath);
		return NULL;
	}

	SDL_GPUShaderCreateInfo shaderInfo = {
		.code_size = codeSize,
		.code = static_cast<Uint8*>(code),
		.entrypoint = entrypoint,
		.format = format,
		.stage = stage,
		.num_samplers = samplerCount,
		.num_storage_textures = storageTextureCount,
		.num_storage_buffers = storageBufferCount,
		.num_uniform_buffers = uniformBufferCount,
	};
	SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
	if (shader == NULL)
	{
		SDL_Log("Failed to create shader!");
		SDL_free(code);
		return NULL;
	}

	SDL_free(code);
	return shader;
}
////////////////////////////////

int game()
{
    Logger::message("TEST MESSAGE");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    Window mainWindow("GAME", 1000, 1000, 0);

    PackageManager dataPackage("data.bin");

    ////////////////////////////////
	// Create the shaders
	SDL_GPUShader* vertexShader = LoadShader(mainWindow.getGPU(), "shader.vert", 0, 0, 0, 0);
	if (vertexShader == NULL)
	{
		SDL_Log("Failed to create vertex shader!");
		return -1;
	}

	SDL_GPUShader* fragmentShader = LoadShader(mainWindow.getGPU(), "shader.frag", 1, 0, 0, 0);
	if (fragmentShader == NULL)
	{
		SDL_Log("Failed to create fragment shader!");
		return -1;
	}

	SDL_GPUColorTargetBlendState colorBlend {
		.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
		.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.color_blend_op = SDL_GPU_BLENDOP_ADD,

		.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
		.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
		.enable_blend = true,
	};

	SDL_GPUColorTargetDescription colorTargetDescription[]
	{
	{
			.format = SDL_GetGPUSwapchainTextureFormat(mainWindow.getGPU(), mainWindow.getWin()),
			.blend_state = colorBlend,
		}
	};

	SDL_GPUVertexBufferDescription vertexBufferDescription[] {{
				.slot = 0,
				.pitch = sizeof(float) * 4,
				.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
				.instance_step_rate = 0,
			}};

	SDL_GPUVertexAttribute vertexAttribute[] {{
				.location = 0,
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
				.offset = 0
			}, {
				.location = 1,
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
				.offset = sizeof(float) * 2
			}};

	SDL_GPUVertexInputState vertexInputState {
			.vertex_buffer_descriptions = vertexBufferDescription,
			.num_vertex_buffers = 1,
			.vertex_attributes = vertexAttribute,
			.num_vertex_attributes = 2,
		};

	// Create the pipeline
	SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.vertex_shader = vertexShader,
		.fragment_shader = fragmentShader,

		.vertex_input_state = vertexInputState,
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,

		.target_info = {
			.color_target_descriptions = colorTargetDescription,
			.num_color_targets = 1,
		},
	};

	Pipeline = SDL_CreateGPUGraphicsPipeline(mainWindow.getGPU(), &pipelineCreateInfo);
	if (Pipeline == NULL)
	{
		SDL_Log("Failed to create pipeline!");
		return -1;
	}

	SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(mainWindow.getGPU());
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

    //CREATE SAMPLE
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
    sample = SDL_CreateGPUSampler(mainWindow.getGPU(), &gpuSampleInfo);


    //CREATE VERTEX/INDEX BUFFERS
    SDL_GPUBufferCreateInfo gpuVertexInfo;
    SDL_zero(gpuVertexInfo);
    gpuVertexInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    gpuVertexInfo.size = sizeof(verticesData.vertices);
    vertexBuffer = SDL_CreateGPUBuffer(mainWindow.getGPU(), &gpuVertexInfo);

	SDL_SetGPUBufferName(mainWindow.getGPU(),
		vertexBuffer,
		"Object vertex"
	);

    SDL_GPUBufferCreateInfo gpuIndexInfo;
    SDL_zero(gpuIndexInfo);
    gpuIndexInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    gpuIndexInfo.size = sizeof(Uint16) * 6;
	indexBuffer = SDL_CreateGPUBuffer(mainWindow.getGPU(), &gpuIndexInfo);

    SDL_GPUTransferBufferCreateInfo bufferInfo;
    SDL_zero(bufferInfo);
    bufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    bufferInfo.size = sizeof(verticesData);
	SDL_GPUTransferBuffer* bufferTransferBuffer = SDL_CreateGPUTransferBuffer(mainWindow.getGPU(), &bufferInfo);

	void* transferData = SDL_MapGPUTransferBuffer(
		mainWindow.getGPU(),
		bufferTransferBuffer,
		false
	);

    SDL_memcpy(transferData, &verticesData, sizeof(verticesData));

    SDL_UnmapGPUTransferBuffer(mainWindow.getGPU(), bufferTransferBuffer);

    SDL_GPUTransferBufferLocation transBuffer;
    SDL_zero(transBuffer);
    transBuffer.transfer_buffer = bufferTransferBuffer;
    transBuffer.offset = 0;

    SDL_GPUBufferRegion buffRegion;
    SDL_zero(buffRegion);
    buffRegion.buffer = vertexBuffer;
    buffRegion.offset = 0;
    buffRegion.size = sizeof(verticesData.vertices);

    SDL_UploadToGPUBuffer(copyPass, &transBuffer, &buffRegion, false);

    transBuffer.offset = sizeof(verticesData.vertices);
    buffRegion.buffer = indexBuffer;
    buffRegion.size = sizeof(verticesData.indexData);

    SDL_UploadToGPUBuffer(copyPass, &transBuffer, &buffRegion, false);

    SDL_EndGPUCopyPass(copyPass);
	SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
    SDL_ReleaseGPUTransferBuffer(mainWindow.getGPU(), bufferTransferBuffer);
    ////////////////////////////////


    for(std::string filename : dataPackage.getFileList())
    {
        if(getExtension(filename) == "png")
        {
            std::vector<uint8_t> data = dataPackage.getFile(filename);
            SDL_IOStream* dataBuffer = SDL_IOFromMem(data.data(), data.size());
            SDL_Surface* surf = IMG_Load_IO(dataBuffer, 1);
			Logger::message("uploading " + filename);
            SDL_GPUTexture* tex = uploadTexture(mainWindow.getGPU(), surf, filename);

            Texture::add(tex, filename);
			Logger::message("uploaded " + filename);
            SDL_DestroySurface(surf);
        }
    }

    //World world(mainWindow.getGPU(), View({1000, 1000}, {0, 0}));

    //world.addImage(new Image({0, 0, 1000, 1000}, "/background.png"));
    //world.addPhyObj(new PhysicsObject({0, 960, 1000, 40}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("/Tile.png")));

    //world.addPhyObj(new Player({500, 920, 40, 40}, PHYOBJ_COLLIDE, new SpriteMap(mainWindow.getGPU(), &dataPackage, "/spritemap.json")));
    //world.startPhysics();

    while (!GameState::gameClosing())
    {
        MouseState::update();
        KeyState::update();

        if(KeyState::key(SDL_SCANCODE_Q) == SDL_EVENT_KEY_DOWN)
            GameState::closeGame();

        ////////////////////////////////
        SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(mainWindow.getGPU());
        SDL_GPUTexture* swapchainTexture;
		if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, mainWindow.getWin(), &swapchainTexture, NULL, NULL))
		{
		SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
		return -1;
		}

		if (swapchainTexture != NULL)
		{
			SDL_GPUColorTargetInfo colorTargetInfo;
			SDL_zero(colorTargetInfo);
			colorTargetInfo.texture = swapchainTexture;
			colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
			colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
			colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

			SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);
			SDL_BindGPUGraphicsPipeline(renderPass, Pipeline);
			SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){ .buffer = vertexBuffer, .offset = 0 }, 1);
			SDL_BindGPUIndexBuffer(renderPass, &(SDL_GPUBufferBinding){ .buffer = indexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
			SDL_BindGPUFragmentSamplers(renderPass, 0, &(SDL_GPUTextureSamplerBinding){ .texture = Texture::get("/spritemap.png"), .sampler = sample }, 1);
			SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);

			SDL_EndGPURenderPass(renderPass);
		}

	    SDL_SubmitGPUCommandBuffer(cmdbuf);

        ////////////////////////////////

        SDL_Delay(1);
        //std::cout << "FPS: " << FPS << "\tPPS:" << PPS << "\r";
    }

    //world.stopPhysics();
    SDL_Quit();

    return 0;
}