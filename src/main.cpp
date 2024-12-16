#include <thread>

extern int game();
extern int debugConsole();

bool gameClosing = false;

int main()
{
    std::thread mainThread(game);

#ifdef DEBUG
    std::thread debugThread(debugConsole);
    debugThread.join();
#endif

    mainThread.join();
    return 0;
}