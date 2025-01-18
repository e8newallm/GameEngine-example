#include <filesystem>
#include <string>
#include <thread>

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
    //Logger::init(std::filesystem::temp_directory_path().append("gamelog.txt"));
    std::set_terminate(unhandledException);

    std::thread mainThread(game);

#ifdef DEBUG
    std::thread debugThread(debugConsole);
    debugThread.join();
#endif

    mainThread.join();
    return 0;
}