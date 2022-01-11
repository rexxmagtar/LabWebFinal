#ifndef _LAB_FINAL_FILE_LOCK_H_
#define _LAB_FINAL_FILE_LOCK_H_

#include <bits/stdc++.h> 

class file_lock 
{
public:    
    enum class lock_mode {
        read, write
    };

private:
    int fd, rw_fd;
    bool is_locked;
    lock_mode mode;

    int do_syslock(bool blocking = true);
    void do_sysunlock();

public:
    file_lock(const std::string &file_name, lock_mode mode);
    ~file_lock();

    void lock();
    bool try_lock();
    void unlock();
};

class file_lock_guard: public file_lock
{
public:
    file_lock_guard(const std::string &file_name, lock_mode mode)
        : file_lock(file_name, mode) { lock(); }
    ~file_lock_guard()               { unlock(); } 
};

#endif // _LAB_FINAL_FILE_LOCK_H_
