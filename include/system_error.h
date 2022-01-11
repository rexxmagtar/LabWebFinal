#ifndef _LAB_FINAL_SYSTEM_ERROR_H_
#define _LAB_FINAL_SYSTEM_ERROR_H_

#include <exception>

class system_error: public std::runtime_error
{
public:
    system_error(const system_error &err)
        : std::runtime_error(err) {}
    system_error(const std::string &what)
        : std::runtime_error(what) {}
    system_error(int err_number)
        : std::runtime_error(std::strerror(err_number)) {}
};

#endif // _LAB_FINAL_SYSTEM_ERROR_H_
