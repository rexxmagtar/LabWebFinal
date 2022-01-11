#ifndef _LAB_FINAL_FCGI_HANDLER_H_
#define _LAB_FINAL_FCGI_HANDLER_H_

#include <fastcgi++/request.hpp>
#include <bits/stdc++.h>

class fcgi_handler: public Fastcgipp::Request<char>
{
private:
#ifndef LAB_FINAL_POST_LENGTH
    static const std::size_t post_length = 
        static_cast<std::size_t>(-1);
#else
    static const std::size_t post_length = 
        static_cast<std::size_t>(LAB_FINAL_POST_LENGTH);
#endif

    bool create_response();
    bool get_one_response(long long id);
    bool get_all_response();
    bool id_response(long long id);
    bool response();
    bool inProcessor();

public:
    fcgi_handler()
        : Fastcgipp::Request<char>(post_length) {}
};

#endif // _LAB_FINAL_FCGI_HANDLER_H_
