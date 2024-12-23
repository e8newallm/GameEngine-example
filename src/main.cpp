#include <filesystem>
#include <thread>

#include "logging.h"

extern int game();
extern int debugConsole();

bool gameClosing = false;

int main()
{
    Logger::init(std::filesystem::temp_directory_path().append("gamelog.txt"));
    std::thread mainThread(game);

#ifdef DEBUG
    std::thread debugThread(debugConsole);
    debugThread.join();
#endif

    mainThread.join();
    return 0;
}