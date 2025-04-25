#include <string>
#include <thread>
#include <SDL3/SDL.h>

#include "logging.h"
#include "graphics.h"

extern int game();
namespace GameEng { extern int debugConsole(); }

using namespace GameEng;

void unhandledException()
{
    try
    {
        std::rethrow_exception(std::current_exception());
    }
    catch (std::exception const &e)
    {
        std::string err = std::string("Unhandled exception: ") + e.what();
        GameEng::Logger::error(err);
    }

    std::abort();
}

int main()
{
    std::set_terminate(unhandledException);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    GPUDevice::GPUDeviceInit();

    std::thread mainThread(game);

#ifdef DEBUG
    std::thread debugThread(GameEng::debugConsole);
    debugThread.join();
#endif

    mainThread.join();

    GPUDevice::GPUDeviceDeinit();
    SDL_Quit();
    return 0;
}