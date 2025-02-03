#include <string>
#include <thread>
#include <SDL3/SDL.h>

#include "logging.h"

extern int game();
extern int debugConsole();

void unhandledException()
{
    try
    {
        std::rethrow_exception(std::current_exception());
    }
    catch (std::exception const &e)
    {
        std::string err = std::string("Unhandled exception: ") + e.what();
        Logger::error(err);
        std::cout << err << std::endl;
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

    std::thread mainThread(game);

#ifdef DEBUG
    std::thread debugThread(debugConsole);
    debugThread.join();
#endif

    mainThread.join();

    SDL_Quit();
    return 0;
}