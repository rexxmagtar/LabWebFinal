#include <fastcgi++/manager.hpp>
#include "webServer.h"

int 
main()
{
    Fastcgipp::Manager<webServer> manager;
    manager.setupSignals();
    manager.listen();
    manager.start();
    manager.join();

    return 0;
}
