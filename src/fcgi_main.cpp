#include <fastcgi++/manager.hpp>
#include "fcgi_handler.h"

int 
main()
{
    Fastcgipp::Manager<fcgi_handler> manager;
    manager.setupSignals();
    manager.listen();
    manager.start();
    manager.join();

    return 0;
}
