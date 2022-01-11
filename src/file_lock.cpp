#include "file_lock.h"
#include "system_error.h"
#include <unistd.h>
#include <sys/file.h>
#include <stdlib.h>

int
file_lock::do_syslock(bool blocking)
{
    int flags = (mode == lock_mode::read ? LOCK_SH : LOCK_EX);
    if (!blocking)
        flags |= LOCK_NB;
    int rw_flags = LOCK_EX;
    if (!blocking)
        rw_flags |= LOCK_NB;
    
    int ret = flock(rw_fd, rw_flags);
    if (0 != ret)
        return ret;
    if (mode == lock_mode::read)
        flock(rw_fd, LOCK_UN);
    ret = flock(fd, flags);
    return ret;
}

void
file_lock::do_sysunlock()
{
    flock(fd, LOCK_UN);
    if (mode == lock_mode::write)
        flock(rw_fd, LOCK_UN);
}

file_lock::file_lock(const std::string &file_name, lock_mode mode)
    : fd(0), is_locked(false), mode(mode)
{
    char *abs_path = realpath(file_name.c_str(), nullptr);
    if (!abs_path)
        throw system_error(errno);
    for (char *p = abs_path; *p; ++p) {
        if ('/' == *p)
            *p = '-';
    }
    char lock_file_path[PATH_MAX] = "/tmp/";
    std::strcat(lock_file_path, abs_path);
    std::free(abs_path);
    rw_fd = open(lock_file_path, O_CREAT|O_RDONLY, 0700);
    if (-1 == rw_fd)
        throw system_error(errno);
    fd = open(file_name.c_str(), O_RDONLY);
    if (-1 == fd) {
        close(rw_fd);
        throw system_error(errno);
    }
}

file_lock::~file_lock()
{
    if (is_locked)
        unlock();
    close(rw_fd);
    close(fd);
}

void
file_lock::lock()
{
    int ret = do_syslock();
    if (-1 == ret)
        throw system_error(errno);
}

bool
file_lock::try_lock()
{
    int ret = do_syslock(false);
    return 0 == ret;
}

void
file_lock::unlock()
{
    do_sysunlock();
}
